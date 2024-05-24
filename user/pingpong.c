#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"
#include "kernel/fs.h"


int
main(int argc,char* argv[]){
    if(argc != 1){
        printf("pingpong:bad input\n");
    }
    int p1[2];              //子->父 读/写。
    int p2[2];              //父->子
    int stat1 = pipe(p1);
    int stat2 = pipe(p2);
    if(stat1 != 0 || stat2 != 0){
        printf("pipe() failed\n");
        exit(0);
    }
    char buf[8];
    if(fork()==0){
        //子
        close(p1[1]);       // 关闭写端
        read(p1[0],buf,8);
        printf("%d: received %s\n",getpid(),buf);
        write(p2[1],"pong",8);
    }else{
        //父
        close(p2[1]);       // 关闭写端
        write(p1[1],"ping",8);
        wait(0);
        read(p2[0],buf,8);
        printf("%d: received %s\n",getpid(),buf);               //中文字符会影响
    }
    exit(0);
}  
