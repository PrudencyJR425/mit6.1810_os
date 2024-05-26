#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  // 检查三个参数 起始虚拟地址 char *、页数 int和存放掩码数据的地址 unsigned int 。
  uint64 va;
  int page_num;
  uint64 access_mask;
  argaddr(0,&va);
  argint(1,&page_num);
  argaddr(2,&access_mask);
  if(page_num < 0 || page_num > 512){
    return -1;
  }
  uint mask=0;
  //TO DO
  //为可以扫描的页面数量设置一个上限.
  //找到正确的pte--遍历页表,参考walk方法
  for(int i=0;i<page_num;i++){
    pte_t* pte = walk(myproc()->pagetable,va+i*PGSIZE,0);
    if(pte && (*pte&PTE_V)&&(*pte&PTE_A)){
      //清除PTE_A
      *pte &= ~PTE_A;
      //设置mask
      mask |= (1<<i);
    }
  }
  // 传出参数:更容易的方法是在内核中存储一个临时缓冲区(中间变量 mask)，并在填充正确的位后将其复制给用户access_mask(通过copyout())。

  copyout(myproc()->pagetable,access_mask,(char*)&mask,sizeof(mask));
  
  return 0;
}
#endif

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
