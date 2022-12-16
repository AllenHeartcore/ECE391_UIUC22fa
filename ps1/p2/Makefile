# Makefile C to ASM
# Aamir Hasan
# 8/30/20

# Yan Miao
# 8/29/21


PROG := edit_dist

CFLAGS += -m32 -Wall -std=c99

.PHONY: clean

all: CFLAGS += -O0
all: $(PROG)

debug: CFLAGS += -O0 -g
debug: $(PROG)

$(PROG): edit_dist_asm.o edit_dist.o utilities.o main.o
	 $(CC) -m32 $^ -o $@

edit_dist_asm.o: edit_dist_asm.S
	$(CC) $(CFLAGS) -c $< -o $@

edit_dist.o: edit_dist.c
	$(CC) $(CFLAGS) -c $< -o $@

utitilites.o: utilities.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROG)
