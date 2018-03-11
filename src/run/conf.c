#include<libebox/run.h>
#include<libebox/errors.h>
#include<strings.h>
#include<string.h>

int ebox_run_newconf(struct ebox_run_conf **cnf, char **argv) {
    //pre-checks
    if(cnf == NULL || argv == NULL) return LIBEBOX_ERR_BADPTR;
    if(*argv == NULL || strlen(*argv) < 1) return LIBEBOX_ERR_BADARG;
    //allocate
    struct ebox_run_conf *conf = autoalloc(struct ebox_run_conf);
    if(conf == NULL) return LIBEBOX_ERR_OOM;
    //populate
    bzero(conf, sizeof(*conf));
    conf->argv = argv;
    *cnf = conf;
    return LIBEBOX_ERR_NONE;
}
static int *ebox_run_conf_selectio(struct ebox_run_conf *cnf, ebox_run_io io) {
    switch(io) {
    case ebox_run_io_stdin:
        return &cnf->in;
        break;
    case ebox_run_io_stdout:
        return &cnf->out;
        break;
    case ebox_run_io_stderr:
        return &cnf->err;
        break;
    default:
        return NULL;
    }
}
int ebox_run_conf_fd(struct ebox_run_conf *cnf, ebox_run_io io, int fd) {
    if(cnf == NULL) return LIBEBOX_ERR_BADPTR;
    if(fd < 0) return LIBEBOX_ERR_BADFD;
    int *fdest = ebox_run_conf_selectio(cnf, io);
    if(fdest == NULL) return LIBEBOX_ERR_BADARG;
    *fdest = fd;
    return LIBEBOX_ERR_NONE;
}
int ebox_run_conf_pipe(struct ebox_run_conf *cnf, ebox_run_io io, \
    struct ebox_poller *poller, struct ebox_poll_state **conn) {
    if(cnf == NULL || conn == NULL || poller == NULL) return LIBEBOX_ERR_BADPTR;
    *conn = NULL;
    int *fdest = ebox_run_conf_selectio(cnf, io);
    if(fdest == NULL) return LIBEBOX_ERR_BADARG;
    struct ebox_poll_state *w, *r;
    int err = ebox_poll_pipe(poller, &w, &r);
    if(err != LIBEBOX_ERR_NONE) return err;
    struct ebox_poll_state *pps, *lps;
    if(io == ebox_run_io_stdin) {
        pps = w;
        lps = r;
    } else {
        pps = r;
        lps = w;
    }
    int pfd = ebox_poll_fd(&pps);
    if(pfd < LIBEBOX_ERR_NONE) {
        ebox_poll_close(w);
        ebox_poll_close(r);
        return pfd;
    }
    *fdest = pfd;
    *conn = lps;
    return LIBEBOX_ERR_NONE;
}
int ebox_run_conf_conn(struct ebox_run_conf *cnf, ebox_run_io io, \
    struct ebox_poll_state **conn) {
    if(cnf == NULL || conn == NULL || *conn == NULL) return LIBEBOX_ERR_BADPTR;
    int *fdest = ebox_run_conf_selectio(cnf, io);
    if(fdest == NULL) return LIBEBOX_ERR_BADARG;
    int pfd = ebox_poll_fd(conn);
    if(pfd < LIBEBOX_ERR_NONE) {
        return pfd;
    }
    *fdest = pfd;
    return LIBEBOX_ERR_NONE;
}
