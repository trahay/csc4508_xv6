#include "types.h"
#include "user.h"

int main() {
  if(fork() == 0) {
    printf(1, "%d - wait...\n", getpid());
    fwait(1, 42);
    printf(1, "go go go!\n");
  } else {
    sleep(500);
    printf(1, "%d - notify...\n", getpid());
    fwake(1, 42);
  }
  exit();
}
