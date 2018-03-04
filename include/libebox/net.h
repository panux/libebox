#include<libebox/core.h>

#ifndef LIBEBOX_NET_H
#define LIBEBOX_NET_H

//use TCP connection
#define LIBEBOX_PROTO_TCP 1
//use UDP connection (unimplemented, reserved for future use)
#define LIBEBOX_PROTO_UDP 2
//use unix domain socket connection
#define LIBEBOX_PROTO_UNIX 3

//connect to server
//port does not matter if proto is LIBEBOX_PROTO_UNIX
int ebox_connect(struct ebox_poller *poller, struct ebox_poll_state **conn, int proto, const char *addr, int port);

//start a server socket
//calls listen with the specified backlog value
//sets the onaccept handler
int ebox_listenandserve(struct ebox_poller *poller, struct ebox_poll_state **conn, int proto, const char *addr, int port, void (*handler)(struct ebox_poll_state*, int), int backlog);

#endif
