#include <netinet/in.h>
#include <sys/select.h>

#define SERV_PORT 9092
#define max(a, b) ((a) > (b) ? (a) : (b))

int Accept(int fd, struct sockaddr* addr, socklen_t* len);
int Bind(int fd, const struct sockaddr* addr, socklen_t len);
int Connect(int fd, const struct sockaddr* addr, socklen_t len);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
int Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
       struct timeval* timeout);

int Read(int fd, void* buf, size_t nbytes);
int Writen(int fd, const void *vptr, size_t n);

void err_sys(const char* str);
void err_quit(const char* str);