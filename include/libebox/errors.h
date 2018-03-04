#ifndef LIBEBOX_ERRORS_H
#define LIBEBOX_ERRORS_H

//no error (returned if no error occurs)
#define LIBEBOX_ERR_NONE 0
//out of memory error
#define LIBEBOX_ERR_OOM -1
//I/O error
#define LIBEBOX_ERR_IO -2
//Something bad with the file descriptor
#define LIBEBOX_ERR_BADFD -3
//bad pointer (e.g. null where not allowed)
#define LIBEBOX_ERR_BADPTR -4
//internal error (e.g. OS rejected option, or unrecognized error)
#define LIBEBOX_ERR_INTERNAL -5
//reached OS limit on resources
#define LIBEBOX_ERR_LIMIT -6
//invalid argument
#define LIBEBOX_ERR_BADARG -7
//insufficient permissions
#define LIBEBOX_ERR_PERM -8

#endif
