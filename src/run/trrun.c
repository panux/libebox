#include<libebox/run.h>
#include<libebox/errors.h>

int ebox_fork(struct ebox_run_ctx *ctx, void (*fun)(void *), void *forkdat, \
    void (*oncomplete)(void *, int, int), void *dat) {
    //run prechecks
    if(ctx == NULL || fun == NULL || oncomplete == NULL) return LIBEBOX_ERR_BADPTR;
    //fork
    int pid = ebox_frun(fun, forkdat);
    if(pid < LIBEBOX_ERR_NONE) return pid;
    //track
    int err = ebox_run_track(ctx, pid, oncomplete, dat);
    if(err != LIBEBOX_ERR_NONE) return err;

    return LIBEBOX_ERR_NONE;
}

int ebox_run(struct ebox_run_ctx *ctx, struct ebox_run_conf *cnf, \
    void (*callback)(void *, int, int), void *dat) {
    //run prechecks
    if(ctx == NULL || cnf == NULL || callback == NULL) return LIBEBOX_ERR_BADPTR;
    //run
    int pid = ebox_run_conf_run(cnf);
    if(pid < LIBEBOX_ERR_NONE) return pid;
    //track
    int err = ebox_run_track(ctx, pid, callback, dat);
}
