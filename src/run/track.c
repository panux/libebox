#include<libebox/run.h>
#include<libebox/errors.h>
#include<string.h>
#include<stdlib.h>

int ebox_run_track(struct ebox_run_ctx *ctx, int pid, \
    void (*callback)(void *, int, int), void *dat) {
    //pre-checks
    if(ctx == NULL || callback == NULL || dat == NULL) return LIBEBOX_ERR_BADPTR;
    if(pid < 1) return LIBEBOX_ERR_BADARG;
    //create PID tracker
    struct ebox_pid_tracker *t = autoalloc(struct ebox_pid_tracker);
    if(t == NULL) return LIBEBOX_ERR_OOM;
    t->pid = pid;
    t->callback = callback;
    t->dat = dat;
    //insert
    if(ctx->pbsize == -1) { //dynamically sized
        //find insert location
        int i;
        for(i = 0; ctx->pids[i] != NULL; i++) {
            if(ctx->pids[i]->pid > pid) {
                break;
            }
        }
        //calculate length
        int len;
        for(len = i; ctx->pids[len] != NULL; len++);
        len++;
        //resize array
        struct ebox_pid_tracker **npids = realloc(ctx->pids, (len+1)*sizeof(t));
        if(npids == NULL) {
            free(t);
            return LIBEBOX_ERR_OOM;
        }
        //move elements
        memmove(npids + i, npids + i + 1, (len-i)*sizeof(t));
        //insert
        npids[i] = t;
    } else if(ctx->pbsize > 0) {
        //find insert location
        int i;
        for(i = 0; i < ctx->pbsize; i++) {
            if(ctx->pids[i] == NULL) {
                break;
            }
            if(ctx->pids[i]->pid > pid) {
                break;
            }
        }
        if(i == ctx->pbsize) {
            free(t);
            return LIBEBOX_ERR_OVERFLOW;
        }
        //move elements
        memmove(ctx->pids + i, ctx->pids + i + 1, (ctx->pbsize-i)*sizeof(t));
        //insert
        ctx->pids[i] = t;
    } else {
        free(t);
        return LIBEBOX_ERR_BADARG;
    }
    return LIBEBOX_ERR_NONE;
}

struct ebox_pid_tracker **ebox_run_search(struct ebox_run_ctx *ctx, int pid) {
    struct ebox_pid_tracker **p;
    for(p = ctx->pids; *p != NULL; p++) {
        if((*p)->pid == pid) {
            //match found
            break;
        }
    }
    return p;
}

int ebox_run_untrack(struct ebox_run_ctx *ctx, int pid, void **dat) {
    //pre-checks
    if(ctx == NULL) return LIBEBOX_ERR_BADPTR;
    if(pid < 1) return LIBEBOX_ERR_BADARG;
    //search array
    struct ebox_pid_tracker **p = ebox_run_search(ctx, pid);
    if(*p == NULL) return LIBEBOX_ERR_NOTFOUND;
    struct ebox_pid_tracker t = **p;
    //remove from array
    struct ebox_pid_tracker **e;
    for(e = p; *e != NULL; e++);    //find end of array
    memmove(p + 1, p, e - p);
    e--;
    //resize array
    if(ctx->pbsize == -1) {
        struct ebox_pid_tracker **na = realloc(ctx->pids, (e - ctx->pids) * sizeof(struct ebox_pid_tracker *));
        if(na != NULL) ctx->pids = na;
    }
    //export dat
    if(dat != NULL) *dat = t.dat;

    return LIBEBOX_ERR_NONE;
}
