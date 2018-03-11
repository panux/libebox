#include<libebox/core.h>
#include<libebox/errors.h>
#include<fcntl.h>
#include<errno.h>

int ebox_poll_fd(struct ebox_poll_state **conn) {
    //pre-checks
    if(conn == NULL || *conn == NULL) return LIBEBOX_ERR_BADPTR;
    int fd = (*conn)->fd;
    if(fd < 0) return LIBEBOX_ERR_BADFD;
    //extract fd and use fcntl
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags < 0) {
        return LIBEBOX_ERR_IO;
    }
    flags &= ~O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1) {
        return LIBEBOX_ERR_IO;
    }
    //buffer cleanup
    ebox_buf_free(&(*conn)->rxbuf);
    ebox_buf_free(&(*conn)->txbuf);
    //free the conn
    free(*conn);
    *conn = NULL;
    //return the fd
    return fd;
}
