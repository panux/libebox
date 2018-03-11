#include<libebox/core.h>

#ifndef LIBEBOX_RUN_H
#define LIBEBOX_RUN_H

//forks and then calls fun with dat
//either returns a PID or returns an error
int ebox_frun(void (*fun)(void *), void *dat);

//enum for different I/O
typedef enum {
    ebox_run_io_stdin,
    ebox_run_io_stdout,
    ebox_run_io_stderr
} ebox_run_io;
//allows overwriting environment variables in the subprocess
struct ebox_run_env_var {
    char *var;  //variable name
    char *val;  //variable value
                //if val == NULL then the variable is unset
};
//settings for running a command
struct ebox_run_conf {
    char **argv;    //null terminated array of arguments, first arg is command
    int in, out, err; //stdin, stdout, stderr
    struct ebox_run_env_var **vars; //environment variable overrides
};

//create a new config using a set of arguments
int ebox_run_newconf(struct ebox_run_conf **cnf, char **argv);
//configures the selected I/O to be set to a file descriptor
int ebox_run_conf_fd(struct ebox_run_conf *cnf, ebox_run_io io, int fd);
//configures a pipe on the selected I/O and sets destination for pipe conn
int ebox_run_conf_pipe(struct ebox_run_conf *cnf, ebox_run_io io, \
    struct ebox_poller *poller, struct ebox_poll_state **conn);
//configures the selected I/O to be set to a conn
int ebox_run_conf_conn(struct ebox_run_conf *cnf, ebox_run_io io, \
    struct ebox_poll_state **conn);
//executes a run conf and returns a PID
int ebox_run_conf_run(struct ebox_run_conf *cnf);


//entries in PID table (do not edit directly - use library functions)
struct ebox_pid_tracker {
    int pid;                            //the PID of the tracked subprocess
    void (*callback)(void *, int, int); //callback run on completion
                                        //args: dat, PID, exit code
    void *dat;                          //user-provided data passed to callback
};
//a buffer for tracking running subprocesses
struct ebox_run_ctx {
    struct ebox_pid_tracker **pids;    //a sorted list of PIDs (NULL-terminated)
    int pbsize;            //the size of the list (or -1 if dynamically resized)
    void (*onother)(int);   //called when a process that is not registered exits
                            //arg: PID
};

//track a subprocess in the ebox_run_ctx
int ebox_run_track(struct ebox_run_ctx *ctx, int pid, \
    void (*callback)(void *, int, int), void *dat);
//untrack a subprocess in the ebox_run_ctx
//this will NOT clean up the user-provided dat value
//if dat is not NULL then it is populated with the dat value
int ebox_run_untrack(struct ebox_run_ctx *ctx, int pid, void **dat);

//fork and call a function, tracking the subprocess
int ebox_fork(struct ebox_run_ctx *ctx, void (*fun)(void *), void *forkdat, \
    void (*oncomplete)(void *, int, int), void *dat);
//run an ebox_run_conf and track the subprocess
int ebox_run(struct ebox_run_ctx *ctx, struct ebox_run_conf *cnf, \
    void (*callback)(void *, int, int), void *dat);


//FOR INTERNAL USE ONLY
struct ebox_pid_tracker **ebox_run_search(struct ebox_run_ctx *ctx, int pid);

#endif
