# Makefile ASM to C
# Aamir Hasan
# 8/30/20

# Yan Miao
# 8/29/21

# Xiang Li
# 8/21/22


PROG := mystery

CFLAGS += -m32 -Wall -std=c99

.PHONY: clean

all: CFLAGS += -O0
all: $(PROG)

debug: CFLAGS += -O0 -g
debug: $(PROG)

$(PROG): mystery_asm.o mystery.o main.o
	 $(CC) -m32 $^ -o $@

mystery_asm.o: mystery_asm.S
	$(CC) $(CFLAGS) -c $< -o $@

mystery.o: mystery.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROG)
