#include<libebox/run.h>
#include<libebox/errors.h>
#include<unistd.h>

int ebox_frun(void (*fun)(void *), void *dat) {
    if(fun == NULL || dat == NULL) return LIBEBOX_ERR_BADPTR;
    int pid = fork();
    switch(pid) {
    case 0:
        fun(dat);
        exit(0);
    case -1:
        return LIBEBOX_ERR_INTERNAL;
    default:
        return pid;
    }
}
