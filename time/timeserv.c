#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage %s <service>\n", argv[0]);
        exit(1);
    }

    int fd = tcp_listen(NULL, argv[1], NULL);
    int clntfd;

    for (;;) {
        clntfd = Accept(fd, NULL, NULL);

        time_t result = time(NULL);
        char* times = ctime(&result);
        Writen(clntfd, times, strlen(times));

        close(clntfd);
    }

    close(fd);

    return 0;
}