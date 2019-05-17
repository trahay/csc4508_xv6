// compile with: gcc -Wall -Werror -g -O3 shm-posix.c -o shm-posix -lrt

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sched.h>
#include <stdatomic.h>

#define dprintf(format, ...)  printf(format, ##__VA_ARGS__)

#define error(format, ...) do {                 \
    fprintf(stderr, format, ##__VA_ARGS__);     \
    exit(1);                                     \
  } while(0)

int main(int argc, char**argv) {

  int fd = shm_open("/plop", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if(fd < 0) {
    error("[%d] unable to create shm\n", getpid());
  }

  int ret = ftruncate(fd, 8192);
  if(ret != 0) {
    error("[%d] ftruncate failed!\n", getpid());
  }

  if(fork()) {
    /* pere */
    int *addr = (int*) 0x10000000;
    int* buffer = mmap(addr, 8192, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(buffer == MAP_FAILED) {
      error("[%d] unable to attach shm\n", getpid());
    }

    dprintf("Le pere attend\n");
    while(atomic_load(&buffer[0]) == 0) sched_yield();
    dprintf("Le pere est débloqué\n");
    sleep(1);
    dprintf("Le pere va débloquer le fils\n");
    atomic_store(&buffer[1], 1);
    
    
    ret = munmap(buffer, 8192);
    if( ret != 0) {
      error("[%d] unable to detach shm\n", getpid());
    }
    wait(NULL);
    ret = shm_unlink("/plop");
    if( ret != 0) {
      error("[%d] unable to destroy shm\n", getpid());
    }
    
    dprintf("fin du pere\n");
  } else {
    /* fils */
    dprintf("Coucou from %d\n", getpid());
    int *addr = (int*) 0x20000000;
    int* buffer = mmap(addr, 8192, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(buffer == MAP_FAILED) {
      error("[%d] unable to attach shm\n", getpid());
    }

    sleep(1);
    dprintf("le fils va debloquer le pere\n");
    atomic_store(&buffer[0], 1);


    dprintf("Le fils attend\n");
    while(atomic_load(&buffer[1]) == 0) sched_yield();
    dprintf("Le fils est débloqué\n");

    ret = munmap(buffer, 8192);
    if(ret != 0) {
      error("[%d] unable to detach shm\n", getpid());
    }
  }

  return 0;
}
