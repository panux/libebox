#include<libebox/app.h>
#include<stdlib.h>
#include<stdio.h>

static void fail(struct ebox_app *app) {
    ebox_help(app);
    exit(1);
}

char *ebox_flagparse_pop(struct ebox_flagparse_ctx *ctx) {
    if(*ctx->args == NULL) {
        fprintf(stderr, "ERROR: Missing value for flag %s\n", ctx->curflag->flagname);
        fail(ctx->app);
    }
    char *arg = *ctx->args;
    ctx->args++;
    return arg;
}

int ebox_flagparse_popnum(struct ebox_flagparse_ctx *ctx, int base) {
    //get flag string
    char *str = ebox_flagparse_pop(ctx);
    if(base == -1) { //autodetect base
        if(*str == '0') {
            str++;
            if(*str == 'x') {
                base = 16;
                str++;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    }
    int n = 0;
    for(;;) {
        //get next character
        char c = *str;
        if(c == '\0') {
            return n;
        }
        //parse digit
        int digit;
        if(c >= '0' && c <= '9') {
            digit = c - '0';
        } else if(c >= 'a' && c <= 'z') {
            digit = 10 + (c - 'a');
        } else if(c >= 'A' && c <= 'Z') {
            digit = 10 + (c - 'A');
        } else {
            fprintf(stderr, "character '%c' is not a digit\n", c);
            //bad character
            fail(ctx->app);
        }
        //check that digit is valid for this base
        if(digit >= base) {
            fprintf(stderr, "Invalid digit '%c' (base %d)\n", c, base);
            fail(ctx->app);
        }
        //append digit valur to least significant end of n
        n *= base;
        n += digit;
        //update string position
        str++;
    }
}

void ebox_flagparse_popss(struct ebox_flagparse_ctx *ctx, char ***ss) {
    int len = 0;
    //get flag string
    char *str = ebox_flagparse_pop(ctx);
    if(*ss == NULL) {
        *ss = malloc(sizeof(char *));
    } else {
        //get length of string set
        for(char **p = *ss; *p != NULL; p++) { len++; }
        //realloc to get space
        *ss = realloc(*ss, sizeof(char*) * (len + 1));
    }
    if(*ss == NULL) {
        fail(ctx->app);
    }
    //place new string into it
    (*ss)[len] = str;
}
