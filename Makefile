HEADERS=assert.h input.h modex.h photo.h photo_headers.h text.h types.h \
	world.h Makefile
OBJS=adventure.o assert.o modex.o input.o photo.o text.o world.o
CFLAGS=-g -Wall

adventure: ${OBJS}
	gcc -g -o adventure ${OBJS} -lpthread -lrt

%.o: %.c ${HEADERS}
	gcc ${CFLAGS} -c -o $@ $<

clean::
	rm -f *.o *~ a.out

clear: clean
	rm -f adventure
