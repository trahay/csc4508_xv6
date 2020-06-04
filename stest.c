#include "types.h"
#include "user.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

int
main(int argc, char *argv[])
{
  unsigned int i, k;

  for(i=0; i<3 && fork(); i++);

  printf(1, "%d starts\n", getpid());

  for(i=0; i<3; i++)
    for(k=0; k<(1<<28); k++);

  printf(1, "%d terminates\n", getpid());

  while(wait() != -1);
  exit();
}

#pragma GCC pop_options
