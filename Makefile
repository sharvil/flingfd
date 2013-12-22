PREFIX = /usr/local
CFLAGS += -Wall -Werror -std=c99 -c -D_GNU_SOURCE

.PHONY:
	all clean install

all:
	$(CC) $(CFLAGS) src/flingfd.c -o src/.flingfd.o
	$(AR) -rc libflingfd.a src/.flingfd.o

clean:
	rm -f libflingfd.a src/.flingfd.o

install: all
	cp src/flingfd.h $(PREFIX)/include
	cp libflingfd.a $(PREFIX)/lib

uninstall:
	rm -f $(PREFIX)/include/flingfd.h $(PREFIX)/lib/libflingfd.a
