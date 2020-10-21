#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <netdb.h>

#define LISTENQ 512
#define SERV_PORT 9092
#define MAX_LINE 4096

#ifndef __cplusplus
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// wrapper functions
int Accept(int fd, struct sockaddr* addr, socklen_t* len);
int Bind(int fd, const struct sockaddr* addr, socklen_t len);
int Connect(int fd, const struct sockaddr* addr, socklen_t len);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
int Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
		struct timeval* timeout);
int Poll(struct pollfd *fds, nfds_t nfds, int timeout);
int Read(int fd, void* buf, size_t nbytes);
int Writen(int fd, const void *vptr, size_t n);
int Recvfrom(int fd, void* buf, size_t nbytes, int flags, struct sockaddr* addr, 
		socklen_t* addr_len);
int Sendto(int fd, const void* buf, size_t nbytes, int flags,
		const struct sockaddr* addr, socklen_t addr_len);
int Getsockopt(int fd, int level, int optname, void* optval, socklen_t* optlen);
int Setsockopt(int fd, int level, int optname, const void* optval, socklen_t optlen);
ssize_t Readline(int fd, void* buf, size_t nbytes);


// error handling functions
void err_sys(const char* str);
void err_quit(const char* str);


// custom functions
struct addrinfo* host_serv(const char* host, const char* serv, int family, int socktype);
int tcp_connect(const char* hostname, const char* service);
int tcp_listen(const char* hostname, const char* service, socklen_t* addrlenp);
int udp_client(const char* hostname, const char* service,
	struct sockaddr** saptr, socklen_t* lenp);
int udp_connect(const char* hostname, const char* service);
int udp_server(const char* hostname, const char* service, socklen_t* lenptr);

void connect_information(const struct sockaddr* addr);

#ifdef __cplusplus
}
#endif