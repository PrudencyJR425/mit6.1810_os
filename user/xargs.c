#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#define MAXLEN 512

void 
fmt_parameter(char* parameters[],char* buf,int *cnt){
    char cur_parameter[MAXLEN];
    int cur_len = 0;
    for(int i=0;i<strlen(buf);i++){
        //printf("%s\n",buf[i]);
        if((buf[i]==' '||buf[i]=='\n')&&cur_len!=0){
            parameters[*cnt] = malloc(MAXLEN);
            memmove(parameters[*cnt],cur_parameter,cur_len);
            parameters[*cnt][cur_len] = 0;
            cur_len = 0;
            (*cnt)++;
        }else{
            cur_parameter[cur_len] = buf[i];
            //printf("%s\n",cur_parameter);
            cur_len++;
        }
    }
}
int
main(int argc,char* argv[]){
    if(argc < 2){
        fprintf(2,"Too few parameters");
        exit(0);
    }
    char* parmeters[MAXARG];
    int cnt = 0;
    for(int i=1;i<argc;i++){
        parmeters[i-1] = argv[i];
        cnt++;
    }
    //printf("parameters[0]:%s\n",parmeters[0]);
    // | 将参数变为标准输入。
    char buf[MAXLEN]; 
    
    while(read(0,buf,MAXLEN)>0){
        if(fork()==0){
            //将标准输入转化为参数,执行exec.
            //printf("%s\n",buf);
            fmt_parameter(parmeters,buf,&cnt);
            exec(parmeters[0],parmeters);
        }else{
            wait(0);
        }
    }
    
    exit(0);
}