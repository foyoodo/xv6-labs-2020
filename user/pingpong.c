#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc > 1) {
    fprintf(2, "Usage: pingpong\n");
    exit(1);
  }

  int p[2];
  char buf[4];

  pipe(p);

  if (fork() == 0) {
    read(p[0], buf, 4);
    fprintf(1, "%d: received %s\n", getpid(), buf);
    write(p[1], "pong", 4);
    exit(0);
  } else {
    write(p[1], "ping", 4);
    wait(0);
    read(p[0], buf, 4);
    fprintf(1, "%d: received %s\n", getpid(), buf);
  }

  exit(0);
}
