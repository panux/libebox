define cc
$(shell cc $(CFLAGS) -M -MT $(1:.c=.o) $(1) | tr -d \\n\\\\) Makefile
	@echo "    CC" $(1:.c=.o)
	@$(CC) -c $(CFLAGS) $(1) -o $(1:.c=.o)
endef
