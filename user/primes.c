#include "../kernel/types.h"
#include "user.h"



// 管道  int fd[2] pipe(fd) arr[0]读 arr[1]写 ，是内核中进程之间通信的机制，这个管道是半双工的，只能一边读，一边写。
// 进程  fork() == 0是子进程{}里面的就是子进程执行的代码块
// 文件描述符 本程序中就是指标记着管道的fd[2]这个小数组

void primes(int read_fd[2]) {
    int prime;
    // 因为每次的数组处理，都会把本次，是第一个数的倍数的全部筛掉，所以第一个数一定是素数
    int flag = read(read_fd[0], &prime, sizeof(prime));
    if (flag == 0){
        exit(0);
    }
    if (prime == -1) {
        exit(0);
    }
    printf("prime %d\n", prime);
 
    int write_fd[2];
    pipe(write_fd);
    if (fork() == 0) {
        close(write_fd[1]);
        close(read_fd[0]);
        primes(write_fd);
    } else {
        // 父进程 此时fork()的返回值不是0,所以是父进程
//        close(read_fd[1]); //对于该进程的文件描述符，写入【2，35】任务已经结束不需要再写了，关闭
        close(write_fd[0]); //父进程只需对，该管道进行写，不用读，所以关闭，本进程持有的读文件描述符;
        int buf;
        while (read(read_fd[0], &buf, sizeof(buf))) {
            if (buf % prime != 0) {
                write(write_fd[1], &buf, sizeof(buf));
            }
        }
        buf = -1;
        write(write_fd[1], &buf, sizeof(buf));
        wait(0);
        exit(0);
    }
}

int main(int argc, char **argv) {
    int data_fd[2];
    pipe(data_fd);

    if (fork() == 0) {
        // 第一个stage子进程
        close(data_fd[1]); //对于父进程的文件描述符，写入[2，35]任务已经结束不需要再写了，关闭
        primes(data_fd);
        exit(0);
    } else {
        // 主进程
        close(data_fd[0]);
        int i ;
        for ( i = 2; i < 36; i++) {
            write(data_fd[1], &i, sizeof(int));
        }
        i = -1;
        write(data_fd[1], &i, sizeof(int));
    }
    wait(0);
    exit(0);
}



