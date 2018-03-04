#include<libebox/errors.h>
#include<libebox/core.h>
#include<libebox/app.h>
#include<libebox/net.h>
#include<stdio.h>
#include<string.h>

char *message;

void ondonesending(struct ebox_poll_state *conn) {
    int err = ebox_poll_close(conn);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to close connection: %d\n", err);
        exit(65);
    }
}

void connect_handler(struct ebox_poll_state* server, int fd) {
    struct ebox_poll_state *conn;
    int err = ebox_poll_new(server->poller, &conn, fd);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to setup connection: %d\n", err);
        return;
    }
    char *dat = strdup(message);
    if(dat == NULL) {
        fprintf(stderr, "Failed to initialize tx buffer: OOM\n");
        return;
    }
    conn->txbuf.base = dat;
    conn->txbuf.dat = dat;
    conn->txbuf.len = strlen(dat);
    conn->txenable = true;
    conn->ondonesending = ondonesending;
    ebox_poll_upd(conn);
}

int main(int argc, char **argv) {
    int port;
    char *addr;
    struct ebox_flag flags[] = {
        ebox_flag_num("p", "port number", &port, 8001),
        ebox_flag_string("addr", "address to bind to", &addr, "0.0.0.0"),
        ebox_flag_string("message", "message to send to clients", &message, "Hello, eBox!"),
    };
    struct ebox_app app = {
        .flags = EBOX_APP_LOADFLAGS(flags),
        .desc = "An example TCP server",
        .version = "v1.0",
    };
    ebox_prerun(&app, argv, argc);
    printf("Starting poll engine\n");
    struct ebox_poller poller;
    int err = ebox_poller_init(&poller, 1);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to init poller: %d\n", err);
        return 65;
    }
    printf("Connecting socket\n");
    struct ebox_poll_state *server;
    err = ebox_listenandserve(&poller, &server, LIBEBOX_PROTO_TCP, addr, port, connect_handler, 65);
    printf("Running server\n");
    for(;;) {
        err = ebox_poll_cycle(&poller);
        if(err != LIBEBOX_ERR_NONE) {
            fprintf(stderr, "Failed to cycle: %d\n", err);
            return 65;
        }
    }
}
