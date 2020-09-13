#include <stddef.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#include "common.h"

char buf[MAX_LINE];

int main(void) {
	int server_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t len;
	int n;

	// create and bind server socket
	server_fd = Socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&server_addr, 0);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	Bind(server_fd, (struct sockaddr*)(&server_addr), sizeof server_addr);

	// echo service
	for (;;) {
		len = sizeof client_addr;
		n = Recvfrom(server_fd, buf, sizeof buf, 0, (struct sockaddr*)(&client_addr), &len);
		Sendto(server_fd, buf, n, 0, (struct sockaddr*)(&client_addr), len);
	}

	close(server_fd);

	return 0;
}