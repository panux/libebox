#include<stdlib.h>
#include<stdbool.h>

#ifndef LIBEBOX_CORE_H
#define LIBEBOX_CORE_H

//allocate a buffer fitting one value of type t
#define autoalloc(t) malloc(sizeof(t))
//allocate an array of n elements of type t
#define autoalloc_arr(t, n) malloc(sizeof(t) * (n))

//buffer for storing arbitrary data
struct ebox_buf {
    char *base;
    char *dat;
    size_t len;
};
//allocates space in the buffer for len bytes of data
//can return LIBEBOX_ERR_OOM or LIBEBOX_ERR_BADPTR (see <libebox/errors.h>)
int ebox_buf_alloc(struct ebox_buf *b, size_t len);
//frees up the memory in the buffer
void ebox_buf_free(struct ebox_buf *b);
//resizes buffer and copies data from moredat into the end of the buffer
//length of moredat is supplied in len
//can return LIBEBOX_ERR_OOM (see <libebox/errors.h>)
int ebox_buf_append(struct ebox_buf *b, char moredat[], size_t len);
//swaps buffer contents
//useful for transferring data between file descriptors
void ebox_buf_swap(struct ebox_buf *a, struct ebox_buf *b);

//state for a file descriptor poller
struct ebox_poller {
    int epoll_fd;
    int maxevents;
};
//a state for the management of I/O on an fd
struct ebox_poll_state {
    struct ebox_poller *poller;     //the poller used for this connection
    bool isSocket;                  //whether or not this is a sockets
    int fd;                         //file descriptor
    void *ctx;                      //user-provided structure with additional data
    union {
        struct {                    //when it is a normal connection
            int epollstat;                                  //the epoll events that are set (used to determine change)
            //txenable automatically turned off if txbuf is empty at update time
            bool rxenable, txenable;                        //whether or not we should be sending/recieving
            struct ebox_buf rxbuf, txbuf;                   //buffers for data being sent/recieved
            void (*onrecv)(struct ebox_poll_state*);        //callback for recieving
            void (*ondonesending)(struct ebox_poll_state*); //callback for sending
            void (*onclosed)(struct ebox_poll_state*);      //callback for when closed
            void (*onerr)(struct ebox_poll_state*); //callback for when connection has errror
        };
        struct {                    //when it is a socket
            void (*onaccept)(struct ebox_poll_state*, int); //callback for accepting connections (second arg is the fd of the conn)
        };
    };
};

//initializes an ebox_poller
//can return LIBEBOX_ERR_BADPTR, LIBEBOX_ERR_LIMIT, LIBEBOX_ERR_OOM, LIBEBOX_ERR_INTERNAL, or LIBEBOX_ERR_BADARG
int ebox_poller_init(struct ebox_poller* poller, int maxevents);
//creates a new ebox_poll_state with the provided file descriptor
//can return LIBEBOX_ERR_BADPTR, LIBEBOX_ERR_BADFD, LIBEBOX_ERR_OOM, or LIBEBOX_ERR_IO
int ebox_poll_new(struct ebox_poller *poller, struct ebox_poll_state **conn, int fd);
//updates epoll mode on a conn
//can return LIBEBOX_ERR_BADPTR or LIBEBOX_ERR_IO
int ebox_poll_upd(struct ebox_poll_state *conn);
//closes a connection
int ebox_poll_close(struct ebox_poll_state *conn);
//runs a polling cycle
int ebox_poll_cycle(struct ebox_poller *poller);
//configures a socket to accept connections
//should call listen before using this
int ebox_poll_serve(struct ebox_poller *poller, struct ebox_poll_state **sock, int sockfd, void (*handler)(struct ebox_poll_state*, int));

#endif
