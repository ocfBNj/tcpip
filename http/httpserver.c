#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "common.h"

#define MAX_CLNT 65535

struct epoll_event ev;
struct epoll_event events[MAX_CLNT];
int epollfd;

void do_get(int fd) {

}

void do_post(int fd) {

}

void do_server(int fd) {
    static char buf[MAX_LINE];
    ssize_t n;

    if ((n = Readline(fd, buf, sizeof buf)) == 0) {
        fputs("disconnect\n", stdout);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        // to do
        return;
    } else {
        // fprintf(stdout, "receive %ld bytes\n", n);
    }

    fputs(buf, stdout);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage %s <service>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_storage addr;
    socklen_t addr_len;

    int listenfd = tcp_listen(NULL, argv[1], NULL);

    int nfds;
    int connfd;

    epollfd = epoll_create(MAX_CLNT);

    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        err_sys("epoll_ctl");
    }

    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_CLNT, -1);
        if (nfds == -1) {
            err_sys("epoll_wait");
        }

        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == listenfd) {
                connfd = Accept(listenfd, (struct sockaddr*)&addr, &addr_len);
                if (connfd == -1) {
                    close(connfd);
                    continue;
                }

                // when a connection comes in, print its information
                connect_information((struct sockaddr*)&addr);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                    close(connfd);
                    continue;
                }
            } else {
                do_server(events[n].data.fd);
            }
        }
    }

    close(listenfd);

    return 0;
}