#include<stdbool.h>

#ifndef LIBEBOX_APP_H
#define LIBEBOX_APP_H

struct ebox_flag;
struct ebox_flagparse_ctx;
struct ebox_app;

struct ebox_flag {
    const char *flagname;   //name of the flag (- if len 1 otherwise --)
    const char *desc;       //description used in help
    union {                 //destination for arg value
        void *ptr;  //used internally
        int *num;
        char **string;
        char ***stringset;
    } dat;
    bool hasdefault;        //whether or not to use a default value
                            //if false, then flag required
    union {                 //default value
        int num;
        char *string;
        char **stringset;
    } def;
    //function called to parse the flag
    void (*handler)(struct ebox_flag *, struct ebox_flagparse_ctx *);
};
//returns a string flag with the specified destination
//if def is a string, then it will be used as the default
//if def is null, then it will be required
struct ebox_flag ebox_flag_string(char *name, char *desc, char **dest, char *def);
//returns a num flag with the specified destination and default value
//if def is -1, then it will be a required flag
struct ebox_flag ebox_flag_num(char *name, char *desc, int *dest, int def);
//returns a string set flag with the specified destination
//if def is non-null, then it will be used as the default
//if def is null, then it will be required
struct ebox_flag ebox_flag_stringset(char *name, char *desc, char ***dest, char **def);

//context for parsing flags
struct ebox_flagparse_ctx {
    char **args;                //arguments, starting with next arg
    struct ebox_flag **flagset; //set of flags to use
    int flagn;                  //number of flags
    struct ebox_flag *curflag;  //current flag being parsed
    struct ebox_app *app;       //pointer to associated app
};

//functions for getting args for a flag
//if no arg available (or bad argument) then prints help and exits
//pop a string argument
char *ebox_flagparse_pop(struct ebox_flagparse_ctx *ctx);
//pop a number argument
int ebox_flagparse_popnum(struct ebox_flagparse_ctx *ctx, int base);
//pop a string argument and append to a string set
void ebox_flagparse_popss(struct ebox_flagparse_ctx *ctx, char ***ss);

//struct containing all data for starting the command-line app
struct ebox_app {
    struct ebox_flag **flags;    //flags for command
    int nflags;
    char *appf;
    char *desc;                 //description of app
    char *version;              //version of app
    char **args;                //arguments parsed from argv
    struct {                    //input/output file descriptors
        int in;     //stdin
        int out;    //stdout
        int err;    //stderr
    } io;
};
//convert flag array to array of flag pointers
struct ebox_flag **ebox_app_loadflags(struct ebox_flag flg[], int n);
#define EBOX_APP_LOADFLAGS(arr) (ebox_app_loadflags((arr), sizeof((arr))/sizeof((arr)[0])))
//parse args
void ebox_prerun(struct ebox_app *app, char **argv, int argc);
//display help
void ebox_help(struct ebox_app *app);

#endif
