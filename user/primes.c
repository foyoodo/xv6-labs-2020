#include "kernel/types.h"
#include "user/user.h"

void
filter(int *pin)
{
  int n, buf, pout[2];
  pipe(pout);
  close(pin[1]);
  if (read(pin[0], &n, sizeof(int))) {
    printf("prime %d\n", n);
    if (fork() == 0) {
      filter(pout);
      exit(0);
    } else {
      close(pout[0]);
      while (read(pin[0], &buf, sizeof(int))) {
        if (buf % n != 0) {
          write(pout[1], &buf, sizeof(int));
        }
      }
      close(pout[1]);
      wait(0);
    }
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  if (argc > 1) {
    fprintf(2, "Usage: primes\n");
    exit(1);
  }

  int p[2];
  pipe(p);

  if (fork() == 0) {
    filter(p);
    exit(0);
  } else {
    close(p[0]);
    for (int i = 2; i <= 35; ++i) {
      write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    wait(0);
  }

  exit(0);
}
