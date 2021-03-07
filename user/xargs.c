#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int
fmtpargs(char **pargs, char *buf, int n)
{
  int t = 0;
  char *h = buf + 1, *p = buf + 1;
  for (int i = 1; i < n; ++i) {
    if (buf[i] == '\\' || buf[i] == '\"') {
      pargs[t] = malloc(p - h);
      memcpy(pargs[t++], h, p++ - h);
      h = ++p;
      ++i;
    } else {
      ++p;
    }
  }
  return t;
}

void
fmtsargs(char **sargs, int at, char *buf, int n)
{
  char *h = buf, *p = buf;
  for (int i = 0; i <= n; ++i) {
    if ((buf[i] == ' ' && (p - h)) || buf[i] == '\n') {
      sargs[at] = malloc(p - h);
      memcpy(sargs[at++], h, p - h);
      h = ++p;
    } else if (buf[i] == ' ') {
      h = ++p;
    } else {
      ++p;
    }
  }
}

int
main(int argc, char *argv[])
{
  char buf[512];
  int n;

  if ((n = read(0, buf, sizeof buf)) < 0) {
    fprintf(2, "xargc: failed to read buf\n");
    exit(1);
  }

  char *args[MAXARG + 1];
  int at = 0;

  if (!strcmp(argv[1], "-n")) {  // xargs -n 1 ...
    for (int i = 3; i < argc; ++i) {
      args[at] = malloc(sizeof argv[i]);
      memcpy(args[at++], argv[i], sizeof argv[i]);
    }
    args[at] = malloc(24);  // assume max length 24
    char *pargs[MAXARG];
    int t = fmtpargs(pargs, buf, n);  // pargs[0...t]
    for (int i = 0; i < t; ++i) {
      memcpy(args[at], pargs[i], sizeof pargs[i]);
      if (fork() == 0) {
        exec(args[0], args);
        exit(0);
      } else {
        wait(0);
      }
    }
  } else {
    for (int i = 1; i < argc; ++i) {
      args[at] = malloc(sizeof argv[i]);
      memcpy(args[at++], argv[i], sizeof argv[i]);
    }
    fmtsargs(args, at, buf, n);
    exec(args[0], args);
  }

  exit(0);
}
