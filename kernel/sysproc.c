#include "types.h"
#include "riscv.h"
#include "procinfo.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
extern void top(void);  // Declaration of the top() function defined in proc.c

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern struct proc proc[NPROC];

uint64
sys_top(void)
{
  uint64 addr;  // user pointer to buffer
  int max;      // max number of entries

  argaddr(0, &addr);
  argint(1, &max);

  struct procinfo info[max];
  int count = 0;

  for (struct proc *p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->state != UNUSED && count < max) {
      info[count].pid = p->pid;
      info[count].state = p->state;
      info[count].ticks = p->ticks;
      safestrcpy(info[count].name, p->name, MAX_NAME_LEN);
      count++;
    }
    release(&p->lock);
  }

  if (copyout(myproc()->pagetable, addr, (char *)info, sizeof(struct procinfo) * count) < 0)
    return -1;

  return count;  // return number of processes copied
}
