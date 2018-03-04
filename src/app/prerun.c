#include<libebox/app.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

struct ebox_flag **ebox_app_loadflags(struct ebox_flag flg[], int n) {
    struct ebox_flag **rval = malloc((n+1) * sizeof(struct ebox_flag *));
    for(int i = 0; i < n; i++) {
        rval[i] = &(flg[i]);
    }
    rval[n] = NULL;
    return rval;
}

//sorting and searching flags
static int ebox_flagComprar(const void *a, const void *b) {
    int cmp = strcmp((*((struct ebox_flag **)a))->flagname, (*((struct ebox_flag **)b))->flagname);
    return cmp;
}
static void ebox_sortFlags(struct ebox_flag **flags, int n) {
    qsort(flags, n, sizeof(struct ebox_flag *), ebox_flagComprar);
}
static struct ebox_flag *ebox_searchFlags(struct ebox_flag **flags, int n, char *flname) {
    struct ebox_flag targ = { .flagname = flname };
    struct ebox_flag *t = &targ;
    struct ebox_flag **f = bsearch(&t, flags, n, sizeof(struct ebox_flag *), ebox_flagComprar);
    if(f == NULL) { return NULL; }
    return *f;
}

//flag parsing
static void ebox_flagproc(struct ebox_flagparse_ctx *ctx, char *flname) {
    struct ebox_flag *fl = ebox_searchFlags(ctx->flagset, ctx->flagn, flname);
    if(fl == NULL) {
        fprintf(stderr, "unrecognized flag: %s\n", flname);
        ebox_help(ctx->app);
        exit(1);
    }
    ctx->curflag = fl;
    fl->handler(fl, ctx);
}
static void ebox_flagparse(struct ebox_flagparse_ctx *ctx) {
    while(*ctx->args != NULL) {
        char *f = ebox_flagparse_pop(ctx);
        if(*f == '-') { //it is a flag
            f++;
            if(*f == '-') {
                f++;
                ebox_flagproc(ctx, f);
            } else {
                //parse each letter individually
                for(; *f != '\0'; f++) {
                    char s[] = {*f, '\0'};
                    ebox_flagproc(ctx, s);
                }
            }
        } else {
            ctx->args--;
            ebox_flagparse_popss(ctx, &ctx->app->args);
        }
    }
}

void ebox_prerun(struct ebox_app *app, char **argv, int argc) {
    struct ebox_flagparse_ctx ctx = {
        .args = argv + 1,
        .flagset = app->flags,
        .app = app
    };
    app->args = malloc(sizeof(char *));
    *app->args = NULL;
    app->appf = *argv;
    int flagn = 0;
    for(struct ebox_flag **f = ctx.flagset; *f != NULL; f++) flagn++;
    ctx.flagn = flagn;
    ebox_sortFlags(ctx.flagset, flagn);
    ebox_flagparse(&ctx);
}
