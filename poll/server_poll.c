// A echo server with poll.

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include <poll.h>

#include "common.h"

char ip[INET_ADDRSTRLEN];
char read_buf[MAX_LINE];

int main() {
	int listen_fd;
	struct pollfd client_fds[1024];
	int client_maxi = 0;

	struct sockaddr_in listen_addr;

	listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&listen_addr, 0, sizeof listen_addr);
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(SERV_PORT);

	Bind(listen_fd, (struct sockaddr*)(&listen_addr), sizeof listen_addr);
	Listen(listen_fd, 5);

	for (int i = 0; i != FD_SETSIZE; i++) {
		client_fds[i].fd = -1;
	}

	client_fds[0].fd = listen_fd;
	client_fds[0].events = POLLIN;

	while (1) {
		Poll(client_fds, client_maxi + 1, -1);

		if (client_fds[0].revents & POLLIN) {
			// ready to accept
			struct sockaddr_in client_addr;
			socklen_t len = sizeof client_addr;
			int client_fd = Accept(listen_fd, (struct sockaddr*)(&client_addr), &len);

			printf("new client: %s, port %d\n",
				inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof ip),
				ntohs(client_addr.sin_port));

			// find a available position
			int i = 1;
			for (; i != 1024; i++) {
				if (client_fds[i].fd < 0) {
					client_fds[i].fd = client_fd;
					client_fds[i].events = POLLIN;
					break;
				}
			}

			if (i == 1024) {
				err_sys("too many clients");
			}

			if (i > client_maxi) {
				client_maxi = i;
			}
		}

		for (int i = 1; i <= client_maxi; i++) {
			if (client_fds[i].fd != -1 && client_fds[i].revents & (POLLIN | POLLERR)) {
				// ready to read
				int n = Read(client_fds[i].fd, read_buf, sizeof read_buf);
				if (n == 0) {
					// read EOF
					close(client_fds[i].fd);
					client_fds[i].fd = -1;
					continue;
				}

				Writen(client_fds[i].fd, read_buf, n);
			}
		}
	}

	close(listen_fd);

	return 0;
}