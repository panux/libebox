lib_LTLIBRARIES = libebox.la
libebox_la_SOURCES = app/flag.c app/help.c app/pop.c app/prerun.c \
    core/buf.c core/poll.c core/fd.c core/pipe.c \
    net/n.h net/connect.c net/listen.c net/tcp.c net/unix.c \
    run/frun.c run/conf.c run/run.c run/track.c run/trrun.c
AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -I$(top_srcdir)/include
