#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

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

uint64
sys_trace(void){
  argint(0,&myproc()->traceMask);      //从陷阱帧中检索第n个系统调用参数并保存。argint(n,&int);
  return 0;
}

uint64
sys_sysinfo(void){
  struct sysinfo sinfo;
  struct proc *p = myproc();
  uint64 addr; // user pointer to struct sysinfo
  argaddr(0, &addr);
  //计算空闲内存大小
  sinfo.freemem = kfree_mem_cnt();
  //计算内存中state不为UNUSED的进程数量
  sinfo.nproc = GetNotUnusedProcNum();
  //函数copyout用于将数据从内核复制到用户提供的地址。

  if(copyout(p->pagetable,addr,(char *)&sinfo,sizeof(sinfo))<0)
    return -1;
  return 0;
}