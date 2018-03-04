#include<libebox/errors.h>
#include "n.h"

int ebox_connect(struct ebox_poller *poller, struct ebox_poll_state **conn, int proto, const char *addr, int port) {
    //check args
    if(poller == NULL || conn == NULL) return LIBEBOX_ERR_BADPTR;
    if(addr == NULL || *addr == '\0') return LIBEBOX_ERR_BADARG;
    if(poller->epoll_fd < 0) return LIBEBOX_ERR_BADFD;

    //initialize output
    *conn = NULL;

    //run proto-appropriate connect function
    int fd = -1;
    int err;
    switch(proto) {
    case LIBEBOX_PROTO_TCP:
        if(port < 1) return LIBEBOX_ERR_BADARG;
        err = ebox_connect_tcp(&fd, addr, port);
        break;
    case LIBEBOX_PROTO_UNIX:
        err = ebox_connect_unix(&fd, addr);
        break;
    default:
        return LIBEBOX_ERR_BADARG;
    }
    if(err != LIBEBOX_ERR_NONE) {
        return err;
    }

    //create conn with fd
    err = ebox_poll_new(poller, conn, fd);
    return err;
}
