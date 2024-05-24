#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find_recursion(char* path,int fd,char* ob){
    struct dirent de;
    struct stat st;
    char buf[512],*p;
    int fd_recur;
    char *tmp;

    while(read(fd,&de,sizeof(de))==sizeof(de)){
        if(de.inum == 0){
            continue;
        }
        strcpy(buf,path);
        p = buf + strlen(path);
        *p++ = '/';
        memmove(p,de.name,DIRSIZ);
        if(strcmp(de.name,".")==0 || strcmp(de.name,"..")==0){
            continue;
        }
        if((fd_recur = open(buf,0)) < 0){
            fprintf(2,"find: cannot open %s\n",buf);
            exit(0);
        }
        if(fstat(fd_recur,&st)<0){
            fprintf(2,"find: cannot stat %s\n",buf);
            close(fd_recur);
            exit(0);            
        }
        switch (st.type)
        {
            case T_DEVICE:
                break;
            case T_FILE:
                //printf("file is %s\n",buf);
                //check是否一致。
            for(tmp=buf+strlen(buf);tmp>=buf && *tmp != '/';tmp--)
            ;
            tmp++;
            //需要及时关闭句柄。
            
            if(strcmp(tmp,ob)==0){
                //printf("tmp:%s,ob:%s\n",tmp,ob);
                char* tmp = buf;
                tmp = tmp + strlen(buf);
                *tmp = '\n';
                write(1, buf, strlen(buf));
            }
            close(fd_recur);
                break;
            case T_DIR:
                find_recursion(buf,fd_recur,ob);
                break;
            default:
                break;
        }
    }
    close(fd);
}

int 
main(int argc,char* argv[]){
    int fd;
    char *path = argv[1];
    char *ob = argv[2];
    char buf[512],*p;

    if(argc < 3){
        fprintf(2,"find:bad input.\n");
        fprintf(2,"try find [path] [filename]\n");
        exit(0);
    }
    //check path
    if((fd = open(path,0))<0){
        fprintf(2,"find: cannot open %s\n",path);
        exit(0);
    }
    //当前一定是个目录文件,遍历它的所有文件。在此之前,将path拷贝到buf中。
    //strlen(path) + 1是给定路径和结尾的斜杠，DIRSIZ是文件名，最后的+ 1是字符串结束符\0
    if(strlen(path)+1+DIRSIZ+1 > sizeof buf){
        printf("find:path is too long.\n");
    }
    strcpy(buf,path);
    p = buf + strlen(path);
    *p++ = '/';
    //递归遍历文件。
    find_recursion(path,fd,ob);
    exit(0);
}