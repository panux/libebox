#include "n.h"
#include<libebox/errors.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>

int ebox_connect_unix(int *fd, const char *addr) {
    if(fd == NULL || addr == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    if((strlen(addr) + 1) > sizeof(saddr.sun_path)) {
        return LIBEBOX_ERR_BADARG;
    }
    int connfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connfd == -1) {
        switch(errno) {
        case EACCES:
            return LIBEBOX_ERR_PERM;
        case EMFILE:
        case ENFILE:
            return LIBEBOX_ERR_LIMIT;
        case ENOBUFS:
        case ENOMEM:
            return LIBEBOX_ERR_OOM;
        default:
            return LIBEBOX_ERR_INTERNAL;
        }
    }
    strcpy(saddr.sun_path, addr);
    if(connect(connfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_un)) == -1) {
        close(connfd);
        return LIBEBOX_ERR_IO;
    }
    *fd = connfd;
    return LIBEBOX_ERR_NONE;
}

int ebox_listen_unix(int *fd, const char *addr) {
    if(fd == NULL || addr == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    if((strlen(addr) + 1) > sizeof(saddr.sun_path)) {
        return LIBEBOX_ERR_BADARG;
    }
    int connfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connfd == -1) {
        switch(errno) {
        case EACCES:
            return LIBEBOX_ERR_PERM;
        case EMFILE:
        case ENFILE:
            return LIBEBOX_ERR_LIMIT;
        case ENOBUFS:
        case ENOMEM:
            return LIBEBOX_ERR_OOM;
        default:
            return LIBEBOX_ERR_INTERNAL;
        }
    }
    strcpy(saddr.sun_path, addr);
    if(bind(connfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_un)) == -1) {
        close(connfd);
        return LIBEBOX_ERR_IO;
    }
    *fd = connfd;
    return LIBEBOX_ERR_NONE;
}
