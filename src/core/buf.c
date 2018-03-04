#include<libebox/core.h>
#include<libebox/errors.h>
#include<string.h>

int ebox_buf_alloc(struct ebox_buf *b, size_t len) {
    if(len == 0) {  //just initialize the buf
        b->dat = NULL;
        b->base = NULL;
        b->len = 0;
        return LIBEBOX_ERR_NONE;
    }
    char *dbuf = malloc(len);
    if(dbuf == NULL) {
        return LIBEBOX_ERR_OOM;
    }
    b->base = dbuf;
    b->dat = dbuf;
    b->len = len;
    return LIBEBOX_ERR_NONE;
}

void ebox_buf_free(struct ebox_buf *b) {
    if(b == NULL) {
        return;
    }
    free(b->base);
    bzero(b, sizeof(*b));
}

int ebox_buf_append(struct ebox_buf *b, char moredat[], size_t len) {
    if(b == NULL || moredat == NULL || (b->len != 0 && b->base == NULL)) {
        return LIBEBOX_ERR_BADPTR;
    }
    //shift over first
    if(b->dat != b->base) {
        memmove(b->base, b->dat, b->len);
    }
    if(b->len == 0) {
        ebox_buf_free(b);
    }
    char *nbuf = realloc(b->base, b->len + len);
    if(nbuf == NULL) {
        return LIBEBOX_ERR_OOM;
    }
    memcpy(nbuf + b->len, moredat, len);
    b->base = nbuf;
    b->dat = nbuf;
    b->len = len;
    return LIBEBOX_ERR_NONE;
}

void ebox_buf_swap(struct ebox_buf *a, struct ebox_buf *b) {
    struct ebox_buf oa = *a;
    struct ebox_buf ob = *b;
    *a = ob;
    *b = oa;
}
