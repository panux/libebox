#include<libebox/app.h>
#include<stdio.h>

int main(int argc, char **argv) {
    int n;
    char *foo;
    struct ebox_flag flags[] = {
        ebox_flag_num("n", "a number", &n, 10),
        ebox_flag_string("foo", "a string", &foo, "bar"),
    };
    struct ebox_app app = {
        .flags = EBOX_APP_LOADFLAGS(flags),
        .desc = "An example app",
        .version = "v1.0",
    };
    ebox_prerun(&app, argv, argc);
    if(n == 1) {
        ebox_help(&app);
        return 0;
    }
    printf("Flags: \n");
    printf("    n = %d\n", n);
    printf("    foo = %s\n", foo);
    if(*app.args != NULL) {
        printf("Args:");
        for(char **a = app.args; *a != NULL; a++) {
            printf("    %s\n", *a);
        }
    }
    return 0;
}
