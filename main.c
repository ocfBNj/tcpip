#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

char ip[INET6_ADDRSTRLEN];

int main(void) {
    int fd;
    struct addrinfo hints;
    struct addrinfo* res;

    bzero(&hints, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    int ret;
    if ((ret = getaddrinfo("ocfbnj.cn", NULL, &hints, &res)) != 0) {
        printf("%s\n", gai_strerror(ret));
        exit(1);
    }

    for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
            printf("%s\n", inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof ip));
        } else if (p->ai_family == AF_INET6) {
			struct sockaddr_in6* addr = (struct sockaddr_in6*)p->ai_addr;
			printf("%s\n", inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof ip));
		} else {
			printf("unknown\n");
		}
    }

    return 0;
}