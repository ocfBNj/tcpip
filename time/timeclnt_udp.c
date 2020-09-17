#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

char buf[MAX_LINE];

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage %s <host> <service>\n", argv[0]);
        exit(1);
    }


    struct sockaddr* sa;
    socklen_t len;

    int fd = udp_client(argv[1], argv[2], &sa, &len);

    Sendto(fd, "\0", 1, 0, sa, len);
    int n = Recvfrom(fd, buf, sizeof buf, 0, sa, &len);

    buf[n] = 0;
    fputs(buf, stdout);

    close(fd);

    return 0;
}