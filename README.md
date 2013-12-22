# flingfd
flingfd is a small, standalone C library to pass file descriptors across processes on Linux.

## Installation
By default, flingfd installs in `/usr/local`. If you want to change where it gets installed, edit the `PREFIX` in the Makefile.

```
  $ make && sudo make install
```

## Documentation
The library consists of two functions declared in `flingfd.h`:
```c
  bool flingfd_simple_send(const char *path, int fd);
  int flingfd_simple_recv(const char *path);
```

When you want to send a file descriptor, call `flingfd_simple_send` and receive it in the other process with `flingfd_simple_recv`. Make sure you use the same `path` argument in both processes -- that determines which process gets the file descriptor.

Here's an example of sending `stdout` to another process:
```c
  #include <flingfd.h>

  void send_my_stdout() {
    int fd = fileno(stdout);
    flingfd_simple_send("/tmp/some_unique_path", fd);
  }
```

And here's the other process writing to the sender's `stdout`:
```c
  #include <flingfd.h>

  void write_to_their_stdout() {
    int fd = flingfd_simple_recv("/tmp/some_unique_path");
    write(fd, "Hello world\n", 12);
  }
```

When you're done writing your code, link against the library with `-lflingfd`.

## Bindings

* Python - https://github.com/soulseekah/pyflingfd

## License
Apache 2.0
