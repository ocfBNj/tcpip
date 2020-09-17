#include <iostream>
#include <cstdlib>
#include <thread>
#include <string_view>
#include <cstring>
#include <cerrno>
#include <filesystem>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

constexpr bool debug = true;
constexpr std::string_view root = "/root";

// thread_local Because for each thread, there is a buffer
static thread_local ssize_t read_cnt;
static thread_local char*   read_ptr;
static thread_local char    read_buf[MAX_LINE];

// Only handle GET requests
enum class req_method {
    GET, UNKNOWN
};

struct request {
    req_method method;
    std::string url;
};

static ssize_t my_read(int fd, char* ptr) {
    if (read_cnt <= 0) {
        // read data to the read_buf
        for (;;) {
            if ((read_cnt = read(fd, read_buf, sizeof read_buf)) < 0) {
                if (errno == EINTR) {
                    continue;
                } else {
                    return -1;
                }
            } else if (read_cnt == 0) {
                return 0;
            }

            read_ptr = read_buf;
            break;
        }
    }

    read_cnt--;
    *ptr = *read_ptr++;

    return 1;
}

static ssize_t read_line(int fd, void* buf, size_t nbytes) {
    ssize_t n, rcnt;
    char c;
    char* ptr = (char*)buf;

    // n = 1 because we will add a '\0' where the buf end
    for (n = 1; n < nbytes; n++) {
        if ((rcnt = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n') {
                break;
            }
        } else if (rcnt == 0) {
            *ptr = 0;
            return n - 1;
        } else {
            return -1;
        }
    }

    *ptr = 0;

    return n;
}

request parsing_http_request_line(std::string_view line) {
    req_method method = req_method::UNKNOWN;

    std::size_t n = line.find(' ', 0);

    std::string_view m = line.substr(0, n);
    if (m == "GET") {
        method = req_method::GET;
    }
    std::string_view url = line.substr(n + 1, line.find(' ', n + 1) - (n + 1));

    if constexpr (debug) {
        std::cout << "request method: " 
                << (method == req_method::GET ? "GET" : "UNKNOWN") 
                << ", request url: " << url << "\n";
    }

    return request{method, std::string(url)};
}

std::string service_http_response_line(std::string_view code) {
    std::string parase;

    if (code == "200") {
        parase = "OK";
    } else if (code == "301") {
        parase = "Moved Permanently";
    } else if (code == "404") {
        parase = "Not Found";
    }

    return std::string("HTTP/1.0 ").append(code).append(" ").append(parase).append("\r\n");
}

std::string service_http_response_header() {
    return "Server: minihttpserver\r\nContent-Type: text/html; charset=UTF-8\r\n";
}

void service_http_response_file(int fd, std::string_view path) {
    std::string response = service_http_response_line("200") + service_http_response_header();
    response.append("Content-Length: " + std::to_string(std::filesystem::file_size(path)) + "\r\n");
    response.append("\r\n");

    std::ifstream is(path.data(), std::ios_base::binary | std::ios::ate);
    if (is.fail()) {
        if constexpr (debug) {
            std::cerr << "Open file " << path << " faild\n";
        }
        
        return;
    }

    Writen(fd, response.data(), response.size());

    std::string data(is.tellg(), '\0');
    is.seekg(0);
    is.read(&data[0], data.size());
    Writen(fd, data.data(), data.size());
}

void service_http_redirect(int fd, std::string_view path) {
    std::string response = service_http_response_line("301") + service_http_response_header();
    response.append(std::string("Location: ") + path.data() + "\r\n");
    response.append("\r\n");

    Writen(fd, response.data(), response.size());
}

void service_http_response_404(int fd, std::string_view path) {
    std::string response = service_http_response_line("404") + service_http_response_header();
    response.append("\r\n");

    Writen(fd, response.data(), response.size());
}

void service_http(int fd) {
    char buf[MAX_LINE];
    request req;

    // Read request line
    if (ssize_t n = read_line(fd, buf, sizeof buf); n > 0) {
        std::string_view request_line(buf, n);

        if constexpr (debug) {
            std::cout << request_line;
        }

        req = parsing_http_request_line(request_line);
    } else if (n == 0) {
        if constexpr (debug) {
            std::cout << "client close connection\n";
        }

        close(fd);
        return;
    } else {
        if constexpr (debug) {
            std::cerr << "read_line() error: " << std::strerror(errno) << "\n";
        }

        close(fd);
        return;
    }

    // Read request header
    for (;;) {
        if (int n = read_line(fd, buf, sizeof buf); n > 0) {
            std::string_view head(buf, n);
            if constexpr (debug) {
                std::cout << head;
            }

            // if (std::size_t i = head.find("Host:", 0); i != std::string_view::npos) {
            //     host = head.substr(i + 6, head.length() - 8);
            // }

            if (req.method == req_method::GET && n == 2) {
                // Read \r\n
                break;
            }
        } else if (n == 0) {
            if constexpr (debug) {
                std::cout << "client close connection\n";
            }

            close(fd);
            return;
        } else {
            if constexpr (debug) {
                std::cerr << "read_line() error: " << std::strerror(errno) << "\n";
            }

            close(fd);
            return;
        }
    }

    if (req.url.front() != '/') {
        req.url = "/" + req.url;
    }

    if (req.url.back() == '/') {
        req.url = req.url + "index.html";
    }

    std::filesystem::path path = std::filesystem::current_path();
    path.concat(root);
    path.concat(req.url);

    std::filesystem::file_status stat = std::filesystem::status(path);

    if (stat.type() == std::filesystem::file_type::regular) {
        // Response file
        service_http_response_file(fd, path.c_str());
    } else if (stat.type() == std::filesystem::file_type::directory) {
        // Redirect
        std::filesystem::path p = std::filesystem::relative(path, std::filesystem::current_path().concat(root));
        service_http_redirect(fd, p.string() + "/");
    } else {
        // Response 404
        service_http_response_404(fd, path.c_str());
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <service>\n";
        std::exit(1);
    }

    int listen_fd = tcp_listen(nullptr, argv[1], nullptr);

    for (;;) {
        struct sockaddr_storage addr;
        socklen_t len = sizeof addr;
        int clnt_fd = Accept(listen_fd, (struct sockaddr*)&addr, &len);

        if constexpr (debug) {
            connect_information((struct sockaddr*)&addr);
        }

        std::thread service{service_http, clnt_fd};
        service.detach();
    }

    close(listen_fd);
}
