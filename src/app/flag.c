#include<libebox/app.h>
#include<stdlib.h>
#include<stdio.h>

static void fail(struct ebox_app *app) {
    ebox_help(app);
    exit(1);
}

static void flag_string_handler(struct ebox_flag *f, struct ebox_flagparse_ctx *ctx) {
    char *str = ebox_flagparse_pop(ctx);
    *(f->dat.string) = str;
}
static void flag_num_handler(struct ebox_flag *f, struct ebox_flagparse_ctx *ctx) {
    int num = ebox_flagparse_popnum(ctx, -1);
    *(f->dat.num) = num;
}
static void flag_ss_handler(struct ebox_flag *f, struct ebox_flagparse_ctx *ctx) {
    ebox_flagparse_popss(ctx, f->dat.stringset);
}

struct ebox_flag ebox_flag_string(char *name, char *desc, char **dest,  char *def) {
    struct ebox_flag fl = {
        .flagname = name,
        .desc = desc,
        .dat.string = dest,
        .hasdefault = def != NULL,
        .def.string = def,
        .handler = flag_string_handler,
    };
    *dest = def;
    return fl;
}
struct ebox_flag ebox_flag_num(char *name, char *desc, int *dest, int def) {
    struct ebox_flag fl = {
        .flagname = name,
        .desc = desc,
        .dat.num = dest,
        .hasdefault = def != -1,
        .def.num = def,
        .handler = flag_num_handler,
    };
    *dest = def;
    return fl;
}
struct ebox_flag ebox_flag_stringset(char *name, char *desc, char ***dest, char **def) {
    struct ebox_flag fl = {
        .flagname = name,
        .desc = desc,
        .dat.stringset = dest,
        .hasdefault = def != NULL,
        .def.stringset = def,
        .handler = flag_ss_handler,
    };
    *dest = def;
    return fl;
}
