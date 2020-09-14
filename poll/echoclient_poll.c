// A echo client with poll.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include <poll.h>

#include "common.h"

void echo_cli(FILE* fp, int sock_fd) {
	static char input_buf[MAX_LINE];
	static char read_buf[MAX_LINE];

	struct pollfd fds[2];
	int stdin_eof = 0;

	fds[0].fd = fileno(fp);
	fds[0].events = POLLIN;
	fds[1].fd = sock_fd;
	fds[1].events = POLLIN;

	while (1) {
		Poll(fds, 2, -1);

		// input is readable
		if (fds[0].revents & POLLIN) {
			if (Read(fileno(fp), input_buf, sizeof input_buf) == 0) {
				shutdown(sock_fd, SHUT_WR);
				stdin_eof = 1;
				continue;
			}

			// write to server
			Writen(sock_fd, input_buf, strlen(input_buf));
		}

		// socket is readable
		if (fds[1].revents & (POLLIN | POLLERR)) {
			// read from server
			int n = Read(sock_fd, read_buf, sizeof read_buf - 1);

			if (n == 0) {
				if (stdin_eof == 0) {
					err_quit("server terminated prematurely");
				} else {
					return;
				}
			}

			read_buf[n] = 0;
			puts(read_buf);
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage %s <ipaddress>\n", argv[0]);
		exit(1);
	}

	int sock_fd;
	struct sockaddr_in serv_addr;

	sock_fd = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof serv_addr);

	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);

	Connect(sock_fd, (struct sockaddr* )(&serv_addr), sizeof serv_addr);

	echo_cli(stdin, sock_fd);

	close(sock_fd);

	return 0;
}