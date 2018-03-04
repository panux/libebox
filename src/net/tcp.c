#include<libebox/errors.h>
#include<netdb.h>
#include<errno.h>
#include<strings.h>
#include<unistd.h>
#include "n.h"

int ebox_connect_tcp(int *fd, const char *addr, int port) {
    if(fd == NULL || addr == NULL) return LIBEBOX_ERR_BADPTR;
    if(port < 1) return LIBEBOX_ERR_BADARG;
    *fd = -1;
    int err = LIBEBOX_ERR_NONE;
    struct hostent *he = gethostbyname(addr);
    if(he == NULL) return LIBEBOX_ERR_IO;
    struct sockaddr_in dest = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = *((struct in_addr *) he->h_addr),
    };
    bzero(dest.sin_zero, sizeof(dest.sin_zero));
    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connfd == -1) {
        switch(errno) {
        case EMFILE:
        case ENFILE:
            err = LIBEBOX_ERR_LIMIT;
            break;
        case EACCES:
            err = LIBEBOX_ERR_PERM;
            break;
        case ENOBUFS:
        case ENOMEM:
            err = LIBEBOX_ERR_OOM;
            break;
        default:
            err = LIBEBOX_ERR_INTERNAL;
            break;
        }
        goto end;
    }
    if(connect(connfd, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) == -1) {
        close(connfd);
        err = LIBEBOX_ERR_IO;
    }
end:
    //return stuff
    if(err == LIBEBOX_ERR_NONE) {
        *fd = connfd;
    }
    return err;
}

int ebox_listen_tcp(int *fd, const char *addr, int port) {
    if(fd == NULL || addr == NULL) {
        return LIBEBOX_ERR_BADPTR;
    }
    if(port < 1) {
        return LIBEBOX_ERR_BADARG;
    }
    *fd = -1;
    int err = LIBEBOX_ERR_NONE;
    struct hostent *he = gethostbyname(addr);
    if(he == NULL) {
        return LIBEBOX_ERR_IO;
    }
    struct sockaddr_in dest = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = *((struct in_addr *) he->h_addr),
    };
    bzero(dest.sin_zero, sizeof(dest.sin_zero));
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1) {
        switch(errno) {
        case EMFILE:
        case ENFILE:
            err = LIBEBOX_ERR_LIMIT;
            break;
        case EACCES:
            err = LIBEBOX_ERR_PERM;
            break;
        case ENOBUFS:
        case ENOMEM:
            err = LIBEBOX_ERR_OOM;
            break;
        default:
            err = LIBEBOX_ERR_INTERNAL;
            break;
        }
        goto end;
    }
    if(bind(sfd, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) == -1) {
        close(sfd);
        err = LIBEBOX_ERR_IO;
    }
end:
    //return stuff
    if(err == LIBEBOX_ERR_NONE) {
        *fd = sfd;
    }
    return err;
}
