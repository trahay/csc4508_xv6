#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"

int sys_mmap() {
  uint addr;
  uint size;
  uint prot;
  uint flag;
  uint fd;
  uint offset;

  if(argint(0, (int*)&addr) < 0 ||
     argint(1, (int*)&size) < 0 ||
     argint(2, (int*)&prot) < 0 ||
     argint(3, (int*)&flag) < 0 ||
     argint(4, (int*)&fd) < 0 ||
     argint(5, (int*)&offset) < 0)
    return -1;

  cprintf("%s(%p, %d, %d, %d, %d, %d)\n", __func__, addr, size, prot, flag, fd, offset);

  addr = PGROUNDDOWN(addr);
  
  struct file* f;
  
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0) {
    cprintf("  %d is not a valid file descriptor", fd);
    return -1;
  }

  if(f->ip->size < offset + size) {
    cprintf("  invalid offset/size %d/%d", offset, size);
    return -1;
  }

  uint npages = (PGROUNDUP(addr + size) - addr) / PGSIZE;
  uint i;
  
  acquiresleep(&f->ip->lock);
  for(i=0; i<npages; i++) {
    char* page = kalloc();
    
    if(!page) {
      cprintf("kalloc failed\n");
      goto err;
    }

    uint  cur = i*PGSIZE;
    uint  rem = size - cur;

    rem = rem < PGSIZE ? rem : PGSIZE;
    
    if(readi(f->ip, page, offset + cur, rem) == -1) {
      cprintf("readi failed\n");
      kfree(page);
      goto err;
    }
    
    if(mappages(myproc()->pgdir, (char*)addr + cur, PGSIZE, V2P(page), PTE_W|PTE_U) == -1) {
      cprintf("mappages failed\n");
      kfree(page);
      goto err;
    }
  }
  
  releasesleep(&f->ip->lock);
  return addr;

 err:
  for(uint j=0; j<i; j++) {
    char* page = unmappage(myproc()->pgdir, (char*)addr + j*PGSIZE);
    kfree(page);
  }

  releasesleep(&f->ip->lock);
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
