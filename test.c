#include "types.h"
#include "fcntl.h"
#include "user.h"
#include "stat.h"
#include "mmap.h"

#define dprintf(format, ...)  printf(1, format, ##__VA_ARGS__)

#define error(format, ...) do {                 \
    printf(2, format, ##__VA_ARGS__);           \
    printf(2, "\n");                            \
    exit();                                     \
  } while(0)

const char name[] = "README";

#define N 10000

int main() {
  int fd = open(name, O_RDONLY);

  if(fd == -1)
    error("unable to open %s", name);

  struct stat stat;

  if(fstat(fd, &stat) == -1)
    error("unable to fstat %s", name);

  dprintf("size: %d bytes\n", stat.size);

  char* addr = mmap((char*)0x10000000, stat.size, 0, 0, fd, 0);

  if(addr == MAP_FAILED)
    error("unable to map %s", name);
  
  for(int i=0; i<(stat.size < N ? stat.size : N); i++)
    dprintf("%c", addr[i]);
  dprintf("\n");
  
  munmap(addr, stat.size);
  close(fd);
  
  exit();
}
