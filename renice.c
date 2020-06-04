#include "types.h"
#include "user.h"


int main(int argc, char**argv) {

  if(argc < 2) {
    printf(1, "Usage: %s pid prio\n", argv[0]);
    exit();
  }
  int pid= atoi(argv[1]);
  int prio = atoi(argv[2]);
  int ret = set_priority(pid, prio);
  if(ret < 0) {
    printf(1, "set_priority(%d, %d) failed\n", pid, prio);
    exit();
  }

  exit();
}
