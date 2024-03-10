// 头文件顺序千万不能错 
#include <kernel/types.h>
#include <user/user.h>
#include <stddef.h>

int main(int argc,char *agrv[]){
    int ftoc_fd[2], ctof_fd[2];
    // pipe_fd[0] 管道的读取端 pipe_fd[1] 管道的写入端
    pipe(ftoc_fd);
    pipe(ctof_fd);
    // 创建缓冲区字符数组，存放传递的信息.
    char buf[8];
    if (fork() == 0){
        // child process
        // 读取father向child传递数据的管道的信息
        read(ftoc_fd[0],buf,4);
        printf("%d: received %s\n",getpid(),buf);
        // 向child向father的管道中写入数据
        write(ctof_fd[1],"pong",strlen("pong"));
    }else{
        // 向father向child的管道中写入数据
        write(ftoc_fd[1],"ping",strlen("ping"));
        wait(NULL);
        read(ctof_fd[0],buf,4);
        printf("%d: received %s\n",getpid(),buf);
    }
    exit(0);
}