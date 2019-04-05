#include <stdio.h>
#include "cpu.h"
#include <stdlib.h>

/**
 * Main
 */
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  struct cpu cpu;

  if (argc < 2)
  {
    fprintf(stderr, "got a ./ls8 filename? \n");
    exit(1);
  }

  cpu_init(&cpu);
  cpu_load(&cpu, argv[1]);
  cpu_run(&cpu);

  return 0;
}
