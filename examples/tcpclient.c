#include<libebox/errors.h>
#include<libebox/core.h>
#include<libebox/app.h>
#include<libebox/net.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

void onrecv(struct ebox_poll_state *in) {
    struct ebox_poll_state *out = in->ctx;
    int err = ebox_buf_append(&out->txbuf, in->rxbuf.dat, in->rxbuf.len);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to transfer buffer: %d\n", err);
        exit(0);
    }
    out->txenable = true;
    ebox_poll_upd(in);
    ebox_poll_upd(out);
    ebox_buf_free(&in->rxbuf);
}

bool morecoming = true;

void onrxclosed(struct ebox_poll_state *in) {
    morecoming = false;
    struct ebox_poll_state *out = in->ctx;
    if(out->txbuf.len == 0) {
        printf("\n");
        exit(0);
    }
}
void ondonesending(struct ebox_poll_state *out) {
    if(!morecoming) {
        printf("\n");
        exit(0);
    }
}

int main(int argc, char **argv) {
    int port;
    char *host;
    struct ebox_flag flags[] = {
        ebox_flag_num("p", "port number", &port, 8001),
        ebox_flag_string("hostname", "hostname or IP of server", &host, "localhost"),
    };
    struct ebox_app app = {
        .flags = EBOX_APP_LOADFLAGS(flags),
        .desc = "An example app that dumps output of a TCP server",
        .version = "v1.0",
    };
    ebox_prerun(&app, argv, argc);
    struct ebox_poller poller;
    int err = ebox_poller_init(&poller, 1);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to init poller: %d\n", err);
        return 65;
    }
    struct ebox_poll_state *conn;
    err = ebox_connect(&poller, &conn, LIBEBOX_PROTO_TCP, host, port);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to connect: %d\n", err);
        return 65;
    }
    struct ebox_poll_state *out;
    err = ebox_poll_new(&poller, &out, STDOUT_FILENO);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to open stdout: %d\n", err);
        return 65;
    }
    conn->ctx = out;
    conn->onrecv = onrecv;
    conn->onclosed = onrxclosed;
    out->ondonesending = ondonesending;
    conn->rxenable = true;
    out->txenable = true;
    err = ebox_poll_upd(conn);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to update conn: %d\n", err);
        return 65;
    }
    err = ebox_poll_upd(out);
    if(err != LIBEBOX_ERR_NONE) {
        fprintf(stderr, "Failed to update stdout: %d\n", err);
        return 65;
    }
    for(;;) {
        err = ebox_poll_cycle(&poller);
        if(err != LIBEBOX_ERR_NONE) {
            fprintf(stderr, "Failed to cycle: %d\n", err);
            return 65;
        }
    }
}
