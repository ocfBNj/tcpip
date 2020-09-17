#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

char buf[16];

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage %s <service>\n", argv[0]);
        exit(1);
    }

    int fd = udp_server(NULL, argv[1], NULL);
    struct sockaddr_storage addr;
    socklen_t addr_len;

    for (;;) {
        addr_len = sizeof addr;
        Recvfrom(fd, buf, sizeof buf, 0, (struct sockaddr*)&addr, &addr_len);

        time_t result = time(NULL);
        char* times = ctime(&result);

        Sendto(fd, times, strlen(times), 0, (struct sockaddr*)&addr, addr_len);
    }

    close(fd);

    return 0;
}