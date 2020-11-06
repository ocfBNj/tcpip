#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

char ip[INET6_ADDRSTRLEN];

int main(void) {
	int fd;
	struct addrinfo hints;
	struct addrinfo* res;

	bzero(&hints, sizeof hints);
	
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	int ret;
	if ((ret = getaddrinfo("ali.ocfbnj.cn", "80", &hints, &res)) != 0) {
		printf("%s\n", gai_strerror(ret));
	}

	for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
		struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
		printf("%s:%d\n",
			inet_ntop(p->ai_family, &addr->sin_addr, ip, sizeof ip),
			ntohs(addr->sin_port));
	}
}