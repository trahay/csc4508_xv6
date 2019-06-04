#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

int sys_mmap() {
  uint  addr;
  uint  size;
  uint  prot;
  uint  flag;
  uint  fd;
  uint  offset;

  if(argint(0, (int*)&addr) < 0 ||
     argint(1, (int*)&size) < 0 ||
     argint(2, (int*)&prot) < 0 ||
     argint(3, (int*)&flag) < 0 ||
     argint(4, (int*)&fd) < 0 ||
     argint(5, (int*)&offset) < 0)
    return -1;

  cprintf("not yet implemented: %s(%p, %d, %d, %d, %d, %d)\n", __func__, addr, size, prot, flag, fd, offset);
  
  return -1;
}

int sys_munmap() {
  char* addr;
  uint  size;

  if(argint(0, (int*)&addr) < 0 ||
     argint(1, (int*)&size) < 0)
    return -1;

  cprintf("not yet implemented: %s(%p, %d)\n", __func__, addr, size);

  return 0;
}
