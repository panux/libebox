#include<libebox/core.h>
#include<libebox/errors.h>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>

int ebox_poller_init(struct ebox_poller* poller, int maxevents) {
    if(poller == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    if(maxevents == -1) {
        maxevents = 4;
    } else if(maxevents < 0) {
        return LIBEBOX_ERR_BADARG;
    }
    int fd = epoll_create1(EPOLL_CLOEXEC);
    if(fd == -1) {
        switch(errno) {
        case EMFILE:
            return LIBEBOX_ERR_LIMIT;
        case ENOMEM:
            return LIBEBOX_ERR_OOM;
        case EINVAL:
        default:
            return LIBEBOX_ERR_INTERNAL;
        }
    }
    poller->epoll_fd = fd;
    poller->maxevents = maxevents;
    return LIBEBOX_ERR_NONE;
}
static int ebox_poll_new_internal(struct ebox_poller *poller, struct ebox_poll_state **conn, int fd, bool issock) {
    //check inputs
    if(poller == NULL || conn == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    *conn = NULL;
    if(poller->epoll_fd == 0) {
        return LIBEBOX_ERR_BADFD;
    }
    //create state object
    struct ebox_poll_state *state = autoalloc(struct ebox_poll_state);
    if(state == NULL) {
        return LIBEBOX_ERR_OOM;
    }
    //set fd as non-blocking
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags < 0) {
        free(state);
        return LIBEBOX_ERR_IO;
    }
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1) {
        free(state);
        return LIBEBOX_ERR_IO;
    }
    //populate state object
    state->poller = poller;
    state->isSocket = issock;
    state->fd = fd;
    state->ctx = NULL;
    if(!issock) {
        state->epollstat = 0;
        state->rxenable = false;
        state->txenable = false;
        ebox_buf_alloc(&state->txbuf, 0);
        ebox_buf_alloc(&state->rxbuf, 0);
        state->onrecv = NULL;
        state->ondonesending = NULL;
        state->onclosed = NULL;
    } else {
        state->onaccept = NULL;
    }
    //save result
    *conn = state;
    return LIBEBOX_ERR_NONE;
}
int ebox_poll_new(struct ebox_poller *poller, struct ebox_poll_state **conn, int fd) {
    return ebox_poll_new_internal(poller, conn, fd, false);
}
int ebox_poll_upd(struct ebox_poll_state *conn) {
    //check that conn is valid
    if(conn == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    if(conn->poller == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    if(conn->poller->epoll_fd < 0) {
        return LIBEBOX_ERR_BADARG;
    }
    if(conn->isSocket) {
        //do nothing
    } else {
        //turn off tx if done sending
        if(conn->txbuf.len == 0) {
            conn->txenable = false;
        }
        //determine epoll events
        int epollstat = 0
            | (conn->rxenable ? EPOLLIN : 0)
            | (conn->txenable ? EPOLLOUT : 0);
        //update if change
        if(epollstat != conn->epollstat) {
            int op;
            //determine if it is adding, removing, or changing
            if(conn->epollstat == 0) {
                op = EPOLL_CTL_ADD;
            } else if(epollstat == 0) {
                op = EPOLL_CTL_DEL;
            } else {
                op = EPOLL_CTL_MOD;
            }
            //try to update
            struct epoll_event ev;
            ev.events = epollstat;
            ev.data.ptr = conn;
            if(epoll_ctl(conn->poller->epoll_fd, op, conn->fd, &ev) == -1) {
                return LIBEBOX_ERR_IO;
            }
            //write updated state
            conn->epollstat = epollstat;
        }
    }
    return LIBEBOX_ERR_NONE;
}
int ebox_poll_cycle(struct ebox_poller *poller) {
    //check that poller is valid
    if(poller == NULL) {
        return LIBEBOX_ERR_BADPTR;
    } else if(poller->maxevents < 0) {
        return LIBEBOX_ERR_BADARG;
    } else if(poller->epoll_fd < 0) {
        return LIBEBOX_ERR_BADFD;
    }
    struct epoll_event evs[poller->maxevents];
ew:;
    int n = epoll_wait(poller->epoll_fd, evs, poller->maxevents, -1);
    if(n == -1) {
        switch(errno) {
            case EBADF:
                return LIBEBOX_ERR_BADFD;
            case EFAULT:
                return LIBEBOX_ERR_BADPTR;
            case EINTR:
                goto ew;
            case EINVAL:
                return LIBEBOX_ERR_BADFD;
            default:
                return LIBEBOX_ERR_INTERNAL;
        }
    }
    for(int i = 0; i < n; i++) {
        struct epoll_event ev = evs[i];
        struct ebox_poll_state *conn = ev.data.ptr;
        if(conn == NULL) {
            return LIBEBOX_ERR_BADPTR;
        }
    fixit:
        if(ev.events & (EPOLLHUP|EPOLLRDHUP)) {
            ebox_poll_close(conn);
        } else if(ev.events & (EPOLLERR|EPOLLPRI)) {
            conn->onerr(conn);
            ebox_poll_close(conn);
        } else if(ev.events & EPOLLOUT) {
            struct ebox_buf *txb = &(conn->txbuf);
            ssize_t n = write(conn->fd, txb->dat, (txb->len > 1024) ? 1024 : txb->len);
            if(n == -1) {
                conn->onerr(conn);
                ebox_poll_close(conn);
                goto y;
            }
            txb->len -= n;
            txb->dat += n;
            if(txb->len == 0) {
                ebox_buf_free(txb);
                if(conn->ondonesending != NULL) {
                    conn->ondonesending(conn);
                }
                ebox_poll_upd(conn);
            }
        } else if(ev.events & EPOLLIN) {
            if(conn->isSocket) {
                printf("Accept\n");
                int fd = accept(conn->fd, NULL, NULL);
                if(errno == -1) {
                    return LIBEBOX_ERR_IO;
                }
                conn->onaccept(conn, fd);
            } else {
                //read up to 1024 bytes
                char buf[1024];
                int n = read(conn->fd, buf, 1024);
                if(n == 0) {
                    ev.events = EPOLLRDHUP;
                    goto fixit;
                }
                if(n == -1) {
                    conn->onerr(conn);
                    ebox_poll_close(conn);
                    goto y;
                }
                int ok = ebox_buf_append(&conn->rxbuf, buf, n);
                if(ok != LIBEBOX_ERR_NONE) {
                    conn->onerr(conn);
                    ebox_poll_close(conn);
                    goto y;
                }
                if(conn->onrecv != NULL) {
                    conn->onrecv(conn);
                }
            }
        }
    y:;
    }
    return LIBEBOX_ERR_NONE;
}
int ebox_poll_close(struct ebox_poll_state *conn) {
    if(conn == NULL || conn->poller == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    if(conn->fd < 0 || conn->poller->epoll_fd < 0) {
        return LIBEBOX_ERR_BADFD;
    }
    if(conn->onclosed != NULL) {
        conn->onclosed(conn);
    }
    free(conn->ctx);
    if(!conn->isSocket) {
        if(conn->epollstat != 0) {
            if(epoll_ctl(conn->poller->epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL) == -1) {
                switch(errno) {
                case EBADF:
                case EINVAL:
                    return LIBEBOX_ERR_BADFD;
                case ENOENT:
                    break;  //it is not registered with epoll
                    //that is ok
                case ENOMEM:
                    return LIBEBOX_ERR_OOM;
                case EPERM: //not sure how this would happen
                    return LIBEBOX_ERR_INTERNAL;
                default:
                    return LIBEBOX_ERR_INTERNAL;
                }
            }
        }
        ebox_buf_free(&conn->rxbuf);
        ebox_buf_free(&conn->txbuf);
    }
    close(conn->fd);
    free(conn);
    return LIBEBOX_ERR_NONE;
}
int ebox_poll_serve(struct ebox_poller *poller, struct ebox_poll_state **sock, int sockfd, void (*handler)(struct ebox_poll_state*, int)) {
    if(poller == NULL || sock == NULL || handler == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    if(sockfd < 0) {
        return LIBEBOX_ERR_BADFD;
    }
    struct ebox_poll_state *s = NULL;
    int err = ebox_poll_new_internal(poller, &s, sockfd, true);
    if(err != LIBEBOX_ERR_NONE) {
        return err;
    }
    s->onaccept = handler;
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = s;
    if(epoll_ctl(s->poller->epoll_fd, EPOLL_CTL_ADD, s->fd, &ev) == -1) {
        free(s);
        return LIBEBOX_ERR_IO;
    }
    *sock = s;
    return LIBEBOX_ERR_NONE;
}
