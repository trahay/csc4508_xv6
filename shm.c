#include "types.h"
#include "user.h"
#include "stdatomic.h"

int main(int argc, char**argv) {

  int fd = shm_create(8192);
  if(fd < 0) {
    error("[%d] unable to create shm\n", getpid());
  }

  if(fork()) {
    /* pere */
    int *buffer = (int*) 0x10000000;
    int ret = shm_attach(fd, buffer);
    if(ret < 0) {
      error("[%d] unable to attach shm\n", getpid());
    }

    dprintf("Le pere attend\n");
    while(atomic_load(&buffer[0]) == 0) ;
    dprintf("Le pere est débloqué\n");
    sleep(1);
    dprintf("Le pere va débloquer le fils\n");
    atomic_store(&buffer[1], 1);
    
    
    ret = shm_detach(fd);
    if( ret < 0) {
      error("[%d] unable to detach shm\n", getpid());
    }
    wait();
    ret = shm_destroy(fd);
    if( ret < 0) {
      error("[%d] unable to destroy shm\n", getpid());
    }
    
    dprintf("fin du pere\n");
  } else {
    /* fils */
    dprintf("Coucou from %d\n", getpid());
    int *buffer = (int*) 0x20000000;
    int ret = shm_attach(fd, buffer);
    if(ret < 0) {
      error("[%d] unable to attach shm\n", getpid());
    }
//
    sleep(1);
    dprintf("le fils va debloquer le pere\n");
    atomic_store(&buffer[0], 1);


    dprintf("Le fils attend\n");
    while(atomic_load(&buffer[1]) == 0) ;
    dprintf("Le fils est débloqué\n");

    ret = shm_detach(fd);
    if(ret < 0) {
      error("[%d] unable to detach shm\n", getpid());
    }
  }

  exit();
  return 0;
}
