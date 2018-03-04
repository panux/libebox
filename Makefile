CFLAGS += -I$(shell pwd)/include -g

ifeq ($(CC),)
CC = cc
endif

export CFLAGS
export CC

all:
	$(MAKE) -C src

test: all
	$(MAKE) -C tests

ex: all
	$(MAKE) -C examples

$(DESTDIR)/usr/lib/libebox.so: src/libebox.so all
	install -D -m 0755 $< $@

install: $(DESTDIR)/usr/lib/libebox.so
