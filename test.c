#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <flingfd.h>

char buf[32];

void send_my_stdout() {
    int fd = fileno(stdout);
    flingfd_simple_send("/tmp/some_unique_path", fd);
}

void write_to_their_stdout() {
    int fd = flingfd_simple_recv("/tmp/some_unique_path");
    int n = snprintf(buf,32,"Hello world from %d\n", getpid());
    write(fd, buf, n+1);
}

int main() {
#ifdef SENDER
    printf("Hello, I'm the sender My pid: %d\n", getpid());
    write_to_their_stdout();
#else
    printf("Hello, I'm receiver. My pid: %d\n", getpid());
    send_my_stdout();
#endif
    return 0;
}
