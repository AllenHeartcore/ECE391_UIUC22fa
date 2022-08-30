CFLAGS=-Wall -g
LDFLAGS=-lm

.PHONY: all
all: ktest utest

ktest: missile-command.o vga.o
	$(CC) $(LDFLAGS) $^ -o $@

utest: umissile-command.o mp1.o mp1_userspace.o mp1-given.o uvga.o
	$(CC) $(LDFLAGS) $^ -o $@ 

mp1.o: mp1.S
	$(CC) $(CFLAGS) -D_USERSPACE -Wa,--gstabs $^ -c -o $@

umissile-command.o: missile-command.c
	$(CC) $(CFLAGS) -D_USERSPACE $^ -c -o $@

uvga.o: vga.c
	$(CC) $(CFLAGS) -D_USERSPACE $^ -c -o $@

clean:
	rm -f *.o ktest utest

