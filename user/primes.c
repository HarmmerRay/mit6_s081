#include <kernel/types.h>
#include <user/user.h>
#include <stddef.h>
void mapping(int n,int pd[]){
    //关闭文件描述符n
    close(n);
    //将管道的读或写端口复制到描述符n上
    //即产生一个n 到 pd[n]的映射
    dup(pd[n]);
    close(pd[0]);
    close(pd[1]);
}
void primes(){
    //定义变量获取管道中的数
    int previous,next;
    int fd[2];
    if (read(0,&previous,sizeof(int))){
        //第一个数一定是素数，直接打印
        printf("prime %d\n",previous);
        // 创建管道
        pipe(fd);
        // 创建子进程
        if (fork() == 0){
            //子进程
            // 子进程将管道的写端口映射到描述符1上面
            mapping(1,fd);
            //循环读取管道中的数据
            while (read(0,&next,sizeof(int))){
                //如果该数不是管道中第一个数的倍数
                if (next % previous != 0){
                    //写入管道
                    write(1,&next,sizeof(int));
                }
            }
        }else{
            //父进程
            //等待子进程把数据全部写入管道
            wait(NULL);
            //父进程将管道的读端口映射到描述符0上
            mapping(0,fd);
            //递归执行此过程
            primes();
        }
    }
}
int mian(int argc,char **argv){饭·
    int pipe_fd[2];
    pipe(pipe_fd);
    if (fork() == 0){
        //子进程
        //子进程将管道的写端口映射到描述符1上
        mapping(1,fd);
        //循环获取[2,35]
        for (int i=0;i<36;i++){

        }
    }
}
