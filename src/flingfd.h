#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Sends a single file descriptor to a receiver. Returns
// true if successful, false otherwise.
bool flingfd_simple_send(const char *path, int fd);

// Receives a single file descriptor from a sender. Returns
// the file descriptor or -1 on error.
int flingfd_simple_recv(const char *path);

typedef struct {
  int fd;
  char *path;
  bool is_bound;
} flingfd_t;

// Creates a new flingfd session for the specified path. You may send or
// receive file descriptors over this session. The returned flingfd_t
// object should be considered an opaque handle and must be released with
// flingfd_close. This function returns NULL and sets errno if a new session
// could not be created.
flingfd_t *flingfd_open(const char *path);

// Sends the given fd over an existing flingfd session. Returns -1 and sets
// errno on error, all other values indicate success.
int flingfd_send(flingfd_t *handle, int fd);

// Receives an fd over an existing flingfd session. Returns -1 and sets errno
// on error, otherwise returns a valid file descriptor.
int flingfd_recv(flingfd_t *handle);

// Closes and releases all resources associated with the flingfd session.
void flingfd_close(flingfd_t **handle);

#ifdef __cplusplus
}
#endif
