include ../ccmk.mk

CFLAGS += -L. -lebox

CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)

all: $(OFILES)

%.o: %.c ../include
	$(CC) $(CFLAGS) $< -o $@
