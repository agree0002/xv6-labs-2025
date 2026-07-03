#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (8*4096)
char data[DATASIZE];

int
main(int argc, char *argv[])
{
  printf("%s\n", data + 16 + PGSIZE);
  exit(1);
}
