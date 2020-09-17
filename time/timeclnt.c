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

    int fd = tcp_connect(argv[1], argv[2]);
    int n;

    for (;;) {
        if ((n = Read(fd, buf, sizeof buf)) == 0) {
            break;
        }
        buf[n] = 0;
        fputs(buf, stdout);
    }

    close(fd);

    return 0;
}