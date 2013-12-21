#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "flingfd.h"

static void flingfd_init_msg_(struct msghdr *msg, struct iovec *iov, char *buf, size_t buf_len);

bool flingfd_simple_send(const char *path, int fd) {
  flingfd_t *handle = flingfd_open(path);
  if (!handle)
    return false;

  bool success = flingfd_send(handle, fd);
  flingfd_close(&handle);
  return success;
}

int flingfd_simple_recv(const char *path) {
  flingfd_t *handle = flingfd_open(path);
  if (!handle)
    return false;

  int ret = flingfd_recv(handle);
  flingfd_close(&handle);
  return ret;
}

flingfd_t *flingfd_open(const char *path) {
  if (!path) {
    errno = EINVAL;
    return NULL;
  }

  int fd = -1;
  char *path_copy = NULL;
  flingfd_t *handle = NULL;

  fd = socket(PF_UNIX, SOCK_DGRAM, 0);
  if (fd == -1)
    goto error;

  path_copy = strdup(path);
  if (!path_copy)
    goto error;

  handle = (flingfd_t *)malloc(sizeof(flingfd_t));
  if (!handle)
    goto error;

  handle->fd = fd;
  handle->path = path_copy;
  handle->is_bound = false;

  return handle;

error:
  if (fd != -1)
    close(fd);

  if (path_copy)
    free(path_copy);

  if (handle)
    free(handle);

  return NULL;
}

int flingfd_send(flingfd_t *handle, int fd) {
  if (!handle || fd == -1) {
    errno = EINVAL;
    return -1;
  }

  if (!handle->is_bound) {
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, handle->path, sizeof(addr.sun_path));
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
    if (connect(handle->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
      return -1;
    handle->is_bound = true;
  }

  struct msghdr msg;
  struct iovec iov;
  char buf[CMSG_SPACE(sizeof(int))];

  flingfd_init_msg_(&msg, &iov, buf, sizeof(buf));

  struct cmsghdr *header = CMSG_FIRSTHDR(&msg);
  header->cmsg_level = SOL_SOCKET;
  header->cmsg_type = SCM_RIGHTS;
  header->cmsg_len = CMSG_LEN(sizeof(int));
  *(int *)CMSG_DATA(header) = fd;

  return sendmsg(handle->fd, &msg, 0);
}

int flingfd_recv(flingfd_t *handle) {
  if (!handle) {
    errno = EINVAL;
    return -1;
  }

  if (!handle->is_bound) {
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, handle->path, sizeof(addr.sun_path));
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
    unlink(addr.sun_path);
    if (bind(handle->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
      return -1;
    handle->is_bound = true;
  }

  struct msghdr msg;
  struct iovec iov;
  char buf[CMSG_SPACE(sizeof(int))];
  flingfd_init_msg_(&msg, &iov, buf, sizeof(buf));

  int ret = recvmsg(handle->fd, &msg, 0);
  if (ret == -1)
    return -1;

  for(struct cmsghdr *header = CMSG_FIRSTHDR(&msg); header != NULL; header = CMSG_NXTHDR(&msg, header))
    if (header->cmsg_level == SOL_SOCKET && header->cmsg_type == SCM_RIGHTS)
      return *(int *)CMSG_DATA(header);

  errno = ENOENT;
  return -1;
}

void flingfd_close(flingfd_t **handle) {
  if (handle == NULL || *handle == NULL)
    return;

  if ((*handle)->fd != -1)
    close((*handle)->fd);

  if ((*handle)->path)
    free((*handle)->path);

  free(*handle);
  *handle = NULL;
}

static void flingfd_init_msg_(struct msghdr *msg, struct iovec *iov, char *buf, size_t buf_len) {
  iov->iov_base = buf;
  iov->iov_len = 1;

  msg->msg_iov = iov;
  msg->msg_iovlen = 1;
  msg->msg_control = buf;
  msg->msg_controllen = buf_len;
  msg->msg_name = NULL;
  msg->msg_namelen = 0;
}
