PREFIX = /usr/local
CFLAGS += -Wall -Werror -std=c99 -D_GNU_SOURCE

.PHONY:
	all clean install uninstall test

all: libflingfd.a test-sender test-receiver

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

libflingfd.a: src/flingfd.o
	$(AR) -rc $@ $^

test-sender: test.c src/flingfd.c
	$(CC) $(CFLAGS) -DSENDER $^ -o $@

test-receiver: test.c src/flingfd.c
	$(CC) $(CFLAGS) $^ -o $@

test: test-sender test-receiver
	./test-sender & </dev/null; MYPID=$$! && \
    ./test-receiver | grep -q "$$MYPID"

clean:
	rm -f libflingfd.a src/flingfd.o test-receiver test-sender

install: libflingfd.a
	cp src/flingfd.h $(PREFIX)/include
	cp libflingfd.a $(PREFIX)/lib

uninstall:
	rm -f $(PREFIX)/include/flingfd.h $(PREFIX)/lib/libflingfd.a
