#include "kernel/types.h"
#include <stddef.h>
#include "user/user.h"
#define MAX_LEN 36

void 
backTracking(int readPort){
    //read 在管道写端关闭时 返回0。
    int read_size,read_res,read_first;
    read_size = read(readPort,&read_res,sizeof(int));
    //读取数据。本进程接收的第一个数据需要打印。一次只读1字节(1个数据)
    //read只有在pipe的写端被关闭、且缓冲区没有数据才会被关闭。
    /*
        printf("pipePort has closed,and read_size=%d\n",read_size);
        if(read_size > 0){
            printf("read data is %d\n",read_res);
        }else{
            exit(0);
        }
    */
    if(read_size > 0){
        read_first = read_res;
        printf("prime %d\n",read_first);
    }else{
        //读取异常,退出。
        exit(0);
    }
    int fd[2];
    pipe(fd);
    //下面考虑筛选、下发。
    int pid = fork();
    if(pid == 0){
        //子进程,递归
        close(fd[1]);
        backTracking(fd[0]);
    }else{
        //父进程 筛选、下发。
        read_size = read(readPort,&read_res,sizeof(int));
        //继续读取数据。缓冲区还有没读完的数据。
        close(fd[0]);
        while (read_size>0)
        {
            //读取到的数据进行筛选,然后下发。
            if(read_res%read_first){
                write(fd[1],&read_res,sizeof(int));
            }
            read_size = read(readPort,&read_res,sizeof(int));
        }
        close(fd[1]);
        close(readPort);
    }
}

int
main(int argc,char* argv[]){
    if(argc != 1){
        printf("primes:too many parameters!\n");
    }
    int fd[2];
    pipe(fd);                   //fd[0]读,fd[1]写。单向
    int pid = fork();
    if(pid==0){
        //子进程,递归。
        close(fd[1]);
        backTracking(fd[0]);
    }else{
        //父进程,筛选、下达。
        close(fd[0]);
        printf("prime 2\n");
        for(int i=2;i<MAX_LEN;i++){
            if(i%2){
                write(fd[1],&i,sizeof(int));
            }
        }
        close(fd[1]);
    }
    wait(NULL);
    exit(0);
}