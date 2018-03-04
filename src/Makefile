CFLAGS += -fPIC
include ../ccmk.mk

all: libebox.so

CFILES = $(wildcard */*.c)
OBJECTS = $(CFILES:.c=.o)

$(foreach cf,$(CFILES),$(eval $(call cc,$(cf))))

libebox.so: $(OBJECTS)
	@echo "    LD" $@
	@$(CC) $(LDFLAGS) -shared $^ -o $@
