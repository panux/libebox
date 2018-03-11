#include<libebox/errors.h>
#include<libebox/core.h>
#include<unistd.h>
#include<errno.h>

int ebox_poll_pipe(struct ebox_poller *poller, struct ebox_poll_state **w, struct ebox_poll_state **r) {
    if(poller == NULL || w == NULL || r == NULL) return LIBEBOX_ERR_BADPTR;
    if(poller->epoll_fd < 0) return LIBEBOX_ERR_BADFD;
    int err = LIBEBOX_ERR_NONE;
    *w = NULL;
    *r = NULL;
    int fdpair[2];
    if(pipe(fdpair) == -1) {
        switch(errno) {
        case EMFILE:
        case ENFILE:
            return LIBEBOX_ERR_LIMIT;
        default:
            return LIBEBOX_ERR_INTERNAL;
        }
    }
    if((err = ebox_poll_new(poller, w, fdpair[1])) == LIBEBOX_ERR_NONE) goto fail;
    if((err = ebox_poll_new(poller, r, fdpair[0])) == LIBEBOX_ERR_NONE) goto fail;
    return LIBEBOX_ERR_NONE;
fail:
    ebox_poll_close(*w);
    ebox_poll_close(*r);
    *w = NULL;
    *r = NULL;
    return err;
}
