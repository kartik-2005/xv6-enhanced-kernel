#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/procinfo.h"

int
main(void)
{
  struct procinfo procs[64]; // Adjust if needed
  int n = top(procs, 64);

  if (n < 0) {
    printf("top: error getting process info\n");
    exit(1);
  }

  printf("%s\t  %s\t  %s\t  %s\n", "PID", "State", "Ticks", "Name");
  printf("--------------------------------------------\n");

  const char *states[] = {
    [0] = "UNUSED  ",
    [1] = "USED    ",
    [2] = "SLEEPING",
    [3] = "RUNNABLE",
    [4] = "RUNNING ",
    [5] = "ZOMBIE  "
  };

  for (int i = 0; i < n; i++) {
    printf("%d\t  %s\t  %d\t  %s\n",
           procs[i].pid,
           states[procs[i].state],
           procs[i].ticks,
           procs[i].name);
  }

  exit(0);
}
