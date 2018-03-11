#define _GNU_SOURCE
#include<libebox/run.h>
#include<libebox/errors.h>
#include<sys/fcntl.h>
#include<unistd.h>

static bool ebox_run_conf_run_setfd(int targfd, int fd) {
    //copy fd to targfd (used for setting STDIN, STDOUT, STDERR)
    if(fd == -1) { return false; }
    if(dup2(fd, targfd) == -1) return true;
    if(close(fd) == -1) return true;
    return false;
}
static void ebox_run_conf_run_fexec_callback(void *dat) {
    struct ebox_run_conf *cnf = dat;
    //set I/O
    if(ebox_run_conf_run_setfd(STDIN_FILENO, cnf->in)
        || ebox_run_conf_run_setfd(STDOUT_FILENO, cnf->out)
        || ebox_run_conf_run_setfd(STDERR_FILENO, cnf->err)
    ) goto fail;
    //set environment variables
    if(cnf->vars != NULL) {
        for(struct ebox_run_env_var **vp = cnf->vars; *vp != NULL; vp++) {
            struct ebox_run_env_var v = **vp;
            if(v.val != NULL) {
                if(setenv(v.var, v.val, 1) == -1) goto fail;
            } else {
                if(unsetenv(v.var) == -1) goto fail;
            }
        }
    }
    //exec
    execvpe(*cnf->argv, cnf->argv, environ);
fail:
    exit(65);
}

static int ebox_run_conf_run_persist(int fd) {
    if(fd == -1) return LIBEBOX_ERR_NONE;
    //disable O_CLOEXEC so that the fd persists
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags < 0) {
        return LIBEBOX_ERR_IO;
    }
    flags &= ~O_CLOEXEC;
    if(fcntl(fd, F_SETFL, flags) == -1) {
        return LIBEBOX_ERR_IO;
    }
    return LIBEBOX_ERR_NONE;
}
int ebox_run_conf_run(struct ebox_run_conf *cnf) {
    //pre-check
    if(cnf == NULL) return LIBEBOX_ERR_BADPTR;
    if(cnf->argv == NULL || *cnf->argv == NULL) return LIBEBOX_ERR_BADARG;
    //persist file descriptors
    int err = ebox_run_conf_run_persist(cnf->in);
    if(err != LIBEBOX_ERR_NONE) return err;
    err = ebox_run_conf_run_persist(cnf->out);
    if(err != LIBEBOX_ERR_NONE) return err;
    err = ebox_run_conf_run_persist(cnf->err);
    if(err != LIBEBOX_ERR_NONE) return err;
    //fork
    int pid = ebox_frun(ebox_run_conf_run_fexec_callback, cnf);
    return pid;
}
