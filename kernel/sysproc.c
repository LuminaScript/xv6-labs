#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#ifdef LAB_PGTBL
#define MAX_PGACCESS 64
extern pte_t *
walk(pagetable_t pagetable, uint64 va, int alloc);
#endif

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
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


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  // get input parameter from user
  uint64 va;
  int n;
  uint64 buf;

  if(argaddr(0, &va) < 0)
    return -1;
  if(argint(1, &n) < 0)
    return -1;
  if(argaddr(2, &buf) < 0)
    return -1;

  if(va >= MAXVA)
    panic("walk");
  if (n <= 0)
    return -1;
  if (n > MAX_PGACCESS)
    n = MAX_PGACCESS;    // restrict to a “safe” maximum

  uint64 result = 0;
  struct proc *p = myproc();
  pagetable_t pt = p->pagetable;

  for (int i = 0; i < n; i++) {
    uint64 curr_va = va + ((uint64)i)*PGSIZE;
    pte_t *pte = walk(pt, curr_va, 0);

    if (pte && (*pte & PTE_V) && (*pte & PTE_A)) result |= (1ULL << i);
    *pte &= ~PTE_A;
  }

  if (copyout(pt, buf, (char *)&result, sizeof(result)) < 0)
    return -1;
    
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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
