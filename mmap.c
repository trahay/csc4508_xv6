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
#include "mmap.h"

int lazymap(uint vaddr, int w) {
  uint seg;
  
  for(seg=0;
      seg<NSEG && (vaddr < myproc()->segs[seg].vaddr || vaddr >= myproc()->segs[seg].vaddr + myproc()->segs[seg].size);
      seg++) { }

  if(seg == NSEG)
    return -1;

  cprintf("== lazy map %p from segment %d (%s access)\n", vaddr, seg, w ? "write" : "read");
  
  uint prot = myproc()->segs[seg].prot;

  /* check protection */
  if(w && !(prot & PROT_WRITE))
    return -1;

  if(!w && !(prot & PROT_READ))
    return -1;

  /* handle concurrency */
  acquiresleep(&myproc()->segs[seg].ip->lock);

  pte_t* pte = walkpgdir(myproc()->pgdir, (char*)vaddr, 0);
  
  /* if another core has already mapped the page, we don't have anything to do */ 
  if(pte && (*pte & PTE_P)) {
    cprintf("==== concurrent access\n");
    goto ok;
  }
  
  char* page = kalloc();

  if(!page) {
    cprintf("kalloc failed\n");
    goto err_release;
  }
  
  uint offset = PGROUNDDOWN(vaddr) - myproc()->segs[seg].vaddr;
  uint rem = myproc()->segs[seg].size - offset;

  rem = rem < PGSIZE ? rem : PGSIZE;

  if(readi(myproc()->segs[seg].ip, page, myproc()->segs[seg].offset + offset, rem) == -1) {
    cprintf("readi failed\n");
    goto err_kfree;
  }
  
  uint rw = (prot & PROT_READ ? PTE_U : 0) | (prot & PROT_WRITE ? PTE_W : 0);

  if(mappages(myproc()->pgdir, (char*)vaddr, PGSIZE, V2P(page), rw) == -1) {
    cprintf("mappages failed\n");
    goto err_kfree;
  }

 ok:
  releasesleep(&myproc()->segs[seg].ip->lock);
  return 0;

 err_kfree:
  kfree(page);
  
 err_release:
  releasesleep(&myproc()->segs[seg].ip->lock);
  return -1;
}

int sys_mmap() {
  uint addr;
  uint size;
  uint prot;
  uint flags;
  uint fd;
  uint offset;

  if(argint(0, (int*)&addr) < 0 ||
     argint(1, (int*)&size) < 0 ||
     argint(2, (int*)&prot) < 0 ||
     argint(3, (int*)&flags) < 0 ||
     argint(4, (int*)&fd) < 0 ||
     argint(5, (int*)&offset) < 0)
    return -1;

  cprintf("%s(%p, %d, %d, %d, %d, %d)\n", __func__, addr, size, prot, flags, fd, offset);

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

  uint seg;
  
  for(seg=0; seg<NSEG && myproc()->segs[seg].vaddr; seg++) { }

  if(seg == NSEG) {
    cprintf("no more segments");
    return -1;
  }

  myproc()->segs[seg].vaddr = addr;
  myproc()->segs[seg].ip = idup(f->ip);
  myproc()->segs[seg].offset = offset;
  myproc()->segs[seg].size = size;
  myproc()->segs[seg].prot = prot;
  myproc()->segs[seg].flags = flags;

  return addr;
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
