#include<libebox/app.h>
#include<string.h>
#include<stdio.h>

void ebox_help(struct ebox_app *app) {
    //print name section
    printf("NAME:\n   %s - %s\n", app->appf, app->desc);
    //print version section
    printf("VERSION:\n   %s\n", app->version);
    //print options
    printf("OPTIONS:\n");
    for(struct ebox_flag **f = app->flags; *f != NULL; f++) {
        printf("   -%s%s  %s%s\n",
            (strlen((*f)->flagname) > 1) ? "-" : "",
            (*f)->flagname,
            (*f)->desc,
            (*f)->hasdefault ? "" : " (required)"
        );
    }
}
