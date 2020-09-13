// A echo server with select.

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>

#include "common.h"

char ip[INET_ADDRSTRLEN];
char read_buf[MAX_LINE];

int main() {
    int listen_fd;
    fd_set set;
    int client_fds[FD_SETSIZE];
    int client_maxi = 0;

    int max_fd;
    struct sockaddr_in listen_addr;

    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

    memset(&listen_addr, 0, sizeof listen_addr);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(SERV_PORT);

    Bind(listen_fd, (struct sockaddr*)(&listen_addr), sizeof listen_addr);
    Listen(listen_fd, 5);

    FD_ZERO(&set);
    FD_SET(listen_fd, &set);

    for (int i = 0; i != FD_SETSIZE; i++) {
        client_fds[i] = -1;
    }

    max_fd = listen_fd;
    while (1) {
        fd_set temp_set = set;
        Select(max_fd + 1, &temp_set, NULL, NULL, NULL);

        if (FD_ISSET(listen_fd, &temp_set)) {
            // ready to accept
            struct sockaddr_in client_addr;
            socklen_t len = sizeof client_addr;
            int client_fd = Accept(listen_fd, (struct sockaddr*)(&client_addr), &len);

            printf("new client: %s, port %d\n",
                inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof ip),
                ntohs(client_addr.sin_port));

            // find a available position
            int i = 0;
            for (; i != FD_SETSIZE; i++) {
                if (client_fds[i] < 0) {
                    client_fds[i] = client_fd;
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                err_sys("too many clients");
            }

            if (i > client_maxi) {
                client_maxi = i;
            }

            // add client to set for select
            FD_SET(client_fd, &set);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
        }

        for (int i = 0; i <= client_maxi; i++) {
            if (client_fds[i] != -1 && FD_ISSET(client_fds[i], &temp_set)) {
                // ready to read
                int n = Read(client_fds[i], read_buf, sizeof read_buf);
                if (n == 0) {
                    // read EOF
                    close(client_fds[i]);
                    FD_CLR(client_fds[i], &set);
                    client_fds[i] = -1;
                    continue;
                }

                Writen(client_fds[i], read_buf, n);
            }
        }
    }

    close(listen_fd);

    return 0;
}