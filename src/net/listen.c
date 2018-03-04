#include<libebox/errors.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include "n.h"

int ebox_listenandserve(struct ebox_poller *poller, struct ebox_poll_state **conn, int proto, const char *addr, int port, void (*handler)(struct ebox_poll_state*, int), int backlog) {
    if(poller == NULL || conn == NULL) return LIBEBOX_ERR_BADPTR;
    if(addr == NULL || handler == NULL) return LIBEBOX_ERR_BADARG;
    if(poller->epoll_fd < 0) return LIBEBOX_ERR_BADFD;
    int fd = -1;
    int err;
    switch(proto) {
    case LIBEBOX_PROTO_TCP:
        if(port < 1) return LIBEBOX_ERR_BADARG;
        err = ebox_listen_tcp(&fd, addr, port);
        break;
    case LIBEBOX_PROTO_UNIX:
        err = ebox_listen_unix(&fd, addr);
        break;
    default:
        err = LIBEBOX_ERR_BADARG;
        break;
    }
    if(err != LIBEBOX_ERR_NONE) return err;
    if(listen(fd, backlog) == -1) {
        close(fd);
        switch(errno) {
        case EADDRINUSE:
            return LIBEBOX_ERR_IO;
        default:
            return LIBEBOX_ERR_INTERNAL;
        }
    }
    return ebox_poll_serve(poller, conn, fd, handler);
}
