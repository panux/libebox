#include<libebox/errors.h>

static const char *errorstrs[] = {
    "no error",
    "out of memory",
    "I/O error",
    "bad file descriptor",
    "bad pointer",
    "internal error",
    "reached OS limit",
    "bad argument",
    "permission denied",
    "overflow",
    "404 not found"
};

const char *ebox_strerror(int err) {
    //prechecks
    if(err > 0) return "not an error";
    err *= -1;  //flip sign bit so it can be used as an index
    if(err >= (sizeof(errorstrs)/sizeof(errorstrs[0]))) return "invalid error";
    return errorstrs[err];
}
