#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

int Accept(int fd, struct sockaddr* addr, socklen_t* len) {
	int n;

	if ((n = accept(fd, addr, len)) < 0) {
		err_sys("accept error");
	}

	return n;
}

int Bind (int fd, const struct sockaddr* addr, socklen_t len) {
	int n;

	if ((n = bind(fd, addr, len)) < 0) {
		err_sys("bind error");
	}

	return n;
}

int Socket(int family, int type, int protocol) {
	int n;

	if ((n = socket(family, type, protocol)) < 0) {
		err_sys("socket error");
	}

	return n;
}

int Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
	   struct timeval* timeout) {
	int n;

	if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
		err_sys("select error");
	}

	return n;
}

int Poll(struct pollfd *fds, nfds_t nfds, int timeout) {
	int n;

	if ((n = poll(fds, nfds, timeout)) == -1) {
		err_sys("poll error");
	}

	return n;
}

int Connect(int fd, const struct sockaddr* addr, socklen_t len) {
	int n;

	if ((n = connect(fd, addr, len)) < 0) {
		err_sys("connect error");
	}

	return n;
}

int Listen(int fd, int backlog) {
	int n;

	if ((n = listen(fd, backlog)) < 0) {
		err_sys("listen error");
	}

	return n;
}

int Read(int fd, void* buf, size_t nbytes) {
	int n;

	if ((n = read(fd, buf, nbytes)) == -1) {
		err_sys("read error");
	}

	return n;
}

int Writen(int fd, const void *vptr, size_t n) {
	size_t nleft = n;
	ssize_t nwriten = 0;
	const char* ptr = (const char*)vptr;

	while (nleft > 0) {
		if ((nwriten = write(fd, ptr, nleft)) == -1) {
			if (nwriten < 0 && errno == EINTR) {
				nwriten = 0;
			} else {
				return -1;
			}
		}

		nleft -= nwriten;
		ptr += nwriten;
	}

	return n;
}

int Recvfrom(int fd, void* buf, size_t nbytes, int flags, struct sockaddr* addr, 
		socklen_t* addr_len) {
	int n;

	if ((n = recvfrom(fd, buf, nbytes, flags, addr, addr_len)) == -1) {
		err_sys("recvfrom error");
	}

	return n;
}

int Sendto(int fd, const void *buf, size_t nbytes, int flags,
		const struct sockaddr* addr, socklen_t addr_len) {
	int n;

	if ((n = sendto(fd, buf, nbytes, flags, addr, addr_len)) == -1) {
		err_sys("sebdto error");
	}

	return n;
}

void err_sys(const char* str) {
	fprintf(stderr, "%s: %s\n", str, strerror(errno));
	exit(1);
}

void err_quit(const char* str) {
	fprintf(stderr, "%s\n", str);
	exit(1);
}