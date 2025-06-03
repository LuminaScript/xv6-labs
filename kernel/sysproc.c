#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  backtrace();
  release(&tickslock);
  return 0;
}

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

uint64
sys_sigalarm(void)
{
  printf("inside sys_sigalarm\n");
  int ticks;
  uint64 f; // The user’s function pointer as an integer

  if(argint(0, &ticks) < 0 || argaddr(1, &f) < 0)
    return -1;

  struct proc *p = myproc();
  p->alarmticks = ticks;//interval
  p->alarmhandler = (void (*)()) f;
  printf("finish sys_sigalarm\n");
  
  return 0;
}

uint64 sys_sigreturn(void)
{
  printf("inside sys_sigreturn\n");
    struct proc *p = myproc();
p->trapframe->epc = p->saved_trap->epc;
p->trapframe->ra  = p->saved_trap->ra;
p->trapframe->sp  = p->saved_trap->sp;
p->trapframe->gp  = p->saved_trap->gp;
p->trapframe->tp  = p->saved_trap->tp;
p->trapframe->t0  = p->saved_trap->t0;
p->trapframe->t1  = p->saved_trap->t1;
p->trapframe->t2  = p->saved_trap->t2;
p->trapframe->t3  = p->saved_trap->t3;
p->trapframe->t4  = p->saved_trap->t4;
p->trapframe->t5  = p->saved_trap->t5;
p->trapframe->t6  = p->saved_trap->t6;
p->trapframe->s0  = p->saved_trap->s0;
p->trapframe->s1  = p->saved_trap->s1;
p->trapframe->s2  = p->saved_trap->s2;
p->trapframe->s3  = p->saved_trap->s3;
p->trapframe->s4  = p->saved_trap->s4;
p->trapframe->s5  = p->saved_trap->s5;
p->trapframe->s6  = p->saved_trap->s6;
p->trapframe->s7  = p->saved_trap->s7;
p->trapframe->s8  = p->saved_trap->s8;
p->trapframe->s9  = p->saved_trap->s9;
p->trapframe->s10 = p->saved_trap->s10;
p->trapframe->s11 = p->saved_trap->s11;
p->trapframe->a0  = p->saved_trap->a0;
p->trapframe->a1  = p->saved_trap->a1;
p->trapframe->a2  = p->saved_trap->a2;
p->trapframe->a3  = p->saved_trap->a3;
p->trapframe->a4  = p->saved_trap->a4;
p->trapframe->a5  = p->saved_trap->a5;
p->trapframe->a6  = p->saved_trap->a6;
p->trapframe->a7  = p->saved_trap->a7;

p->in_handler = 0;

  return 0;
}