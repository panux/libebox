#include<libebox/net.h>

#ifndef LIBEBOX_N_H
#define LIBEBOX_N_H

//connect to address and store fd into fd
//returns error
int ebox_connect_tcp(int *fd, const char *addr, int port);
int ebox_connect_unix(int *fd, const char *addr);

//set up socket fd and store into fd
//returns error
//does not run listen
int ebox_listen_tcp(int *fd, const char *addr, int port);
int ebox_listen_unix(int *fd, const char *addr);

#endif
