// #include <json-c/json.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUF_SIZE 2048

typedef enum {
  hyc_ERROR,
  hyc_FATAL,
  hyc_WARN,
  hyc_INFO,
} hyc_log_level;

void hyc_log(hyc_log_level level, char *fmt, ...) {
  char *lvl_str;
  FILE *lvl_fd;

  switch (level) {
  case hyc_FATAL:
    lvl_str = "FATAL ERROR";
    lvl_fd = stderr;
    break;
  case hyc_ERROR:
    lvl_str = "ERROR";
    lvl_fd = stderr;
    break;
  case hyc_WARN:
    lvl_str = "WARNING";
    lvl_fd = stdout;
    break;
  case hyc_INFO:
    lvl_str = "INFO";
    lvl_fd = stdout;
    break;
  }
  fprintf(lvl_fd, "[%s] ", lvl_str);
  va_list args;
  va_start(args, fmt);
  vfprintf(lvl_fd, fmt, args);
  va_end(args);
  fprintf(lvl_fd, "\n");
  if (level == hyc_FATAL) {
    abort();
  }
}

void *hyc_alloc(size_t bytes) {
  void *r = malloc(bytes);
  if (r == NULL) {
    hyc_log(hyc_FATAL, "Ran out of memory");
    abort();
  }
  return r;
}
void *hyc_realloc(void *ptr, size_t bytes) {
  void *r = realloc(ptr, bytes);
  if (r == NULL) {
    hyc_log(hyc_FATAL, "Ran out of memory");
    abort();
  }
  return r;
}

char *hyc_get_socket_path() {
  char *id = getenv("HYPRLAND_INSTANCE_SIGNATURE");

  char *buf = hyc_alloc((strlen(id) + 23) * sizeof(char));

  strcpy(buf, "/tmp/hypr/");

  strcat(buf, id);
  strcat(buf, "/.socket.sock");
  return buf;
}

char *hyc_call_socket(char *request) {
  struct sockaddr_un addr;
  const char *SPATH = hyc_get_socket_path();
  printf("%s\n", SPATH);

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  printf("Client socket fd = %d\n", sfd);

  if (sfd == -1) {
    hyc_log(hyc_ERROR, "socket fd is zesty");
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SPATH, sizeof(addr.sun_path) - 1);

  if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) ==
      -1) {
    hyc_log(hyc_FATAL, "connect");
  }

  size_t data_len = strlen(request) * sizeof(char);

  if (write(sfd, request, data_len) != data_len) {
    hyc_log(hyc_FATAL, "partial/failed write");
  }

  char *buf = hyc_alloc(BUF_SIZE);
  int numRead = 0;
  int index = 0;
  while ((numRead = read(sfd, buf + index, BUF_SIZE)) > 0) {
    index += numRead;
    buf = hyc_realloc(buf, index + BUF_SIZE);
  }

  return buf;
}

int main(void) {
  char *clients_str = hyc_call_socket("j/clients");
  // json_object *root =
}
