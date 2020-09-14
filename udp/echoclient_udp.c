#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#include "common.h"

char buf[MAX_LINE];

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage %s <ipaddress>\n", argv[0]);
		exit(1);
	}

	int server_fd;
	struct sockaddr_in server_addr;
	int n;

	// create and connect server socket
	server_fd = Socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&server_addr, 0);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	Connect(server_fd, (struct sockaddr*)(&server_addr), sizeof server_addr);

	// get echo service
	for (;;) {
		if (fgets(buf, sizeof buf, stdin) == NULL) {
			break;
		}

		Writen(server_fd, buf, strlen(buf));
		n = Read(server_fd, buf, sizeof buf - 1);
		buf[n] = 0;
		fputs(buf, stdout);
	}

	close(server_fd);

	return 0;
}