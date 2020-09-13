// A echo client with select.

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

#include "common.h"

void echo_cli(FILE* fp, int sock_fd) {
	static char input_buf[MAX_LINE];
	static char read_buf[MAX_LINE];

	int max_fd;
	fd_set set;
	int stdin_eof = 0;

	FD_ZERO(&set);

	while (1) {
		if (stdin_eof == 0) {
			FD_SET(fileno(fp), &set);
		}
		FD_SET(sock_fd, &set);
		max_fd = max(fileno(fp), sock_fd);
		Select(max_fd + 1, &set, NULL, NULL, NULL);

		// input is readable
		if (FD_ISSET(fileno(fp), &set)) {
			if (Read(fp, input_buf, sizeof input_buf) == 0) {
				shutdown(sock_fd, SHUT_WR);
				FD_CLR(fileno(fp), &set);
				stdin_eof = 1;
				continue;
			}

			// write to server
			Writen(sock_fd, input_buf, strlen(input_buf));
		}

		// socket is readable
		if (FD_ISSET(sock_fd, &set)) {
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