#include "../kernel/types.h"
#include "user.h"

// 管道  int fd[2] pipe(fd) arr[0]读 arr[1]写 ，是内核中进程之间通信的机制，这个管道是半双工的，只能一边读，一边写。
// 进程  fork() == 0是子进程{}里面的就是子进程执行的代码块
// 文件描述符 本程序中就是指标记着管道的fd[2]这个小数组
// 程序整体实现思路： 实现打印35以内的素数的程序，通过一层层的子进程，每层子进程把是自己倍数的数(这些数肯定不是素数)给剔除掉，把剩下的数再一层层的传递下去
// 首先 定义主进程中的管道，进行数据[2,35]的写入，随后关闭主进程中，此管道的读写口。
// 然后 创建子进程将data_fd[2]数组地址传到primes，此时子进程对于此管道的读写口是完全复制的父进程的，都处于打开的状态。
// 其次 读取第一个prime数据，打印出来，创建新的write_fd[2]，新的管道，将筛选过后的数据写入进去，传给下一个子进程
// 最后 当prime是-1时候即exit(0)跳出
void primes(int read_fd[2]) {
    int prime;
    // 因为每次的数组处理，都会把本次，是第一个数的倍数的全部筛掉，所以第一个数一定是素数
    read(read_fd[0], &prime, sizeof(prime));
    if (prime == -1) {
        printf("我是prime:%d 我要退出了",prime);
        exit(0);  //已经执行到这里了，但是就是退出不了
    }
    printf("prime %d\n", prime);

    int write_fd[2];
    pipe(write_fd);

    if (fork() == 0) {
        close(write_fd[1]);
        close(read_fd[0]);
        primes(write_fd);
    } else {
        close(write_fd[0]); //父进程只需对，该管道进行写，不用读，所以关闭，本进程持有的读文件描述符;
        int buf;
        printf("-----pid:%d-----\n",getpid());
        while (read(read_fd[0], &buf, sizeof(buf)) && buf != -1) {
            // 哇！ 终于找到问题所在了！ 因为这个while判断中没有加入&& buf != -1 但是为什么呢？
            // 哎哟我！ 打开新知识了 pipe()函数返回的两个文件描述符即fd[0]读fd[1]写，这两个都是阻塞的，就拿fd[0]读文件描述符来说，如果管道中没有数据时，read(fd[0])读取这个管道的数据时候
            // 就会阻塞在那里，导致看着好像是while循环阻塞了，实则是read()函数阻塞了。此时该进程卡顿在这里，所以加 buf != -1 当读取到最后的-1时候，while循环条件不符 退出。
            // 然后[2,35]每次筛选过的数据正常地写入,写完之后，再向管道中写入 -1 ，保证下面开辟的子进程读取的管道中，有这一标志位,可以正常结束进程。
            if (buf % prime != 0) {
                printf("****prime:%d buf:%d****\n",prime,buf);
                write(write_fd[1], &buf, sizeof(buf));
            }
        }
        printf("***pid:%d 循环结束***\n",getpid());
        buf = -1;
        write(write_fd[1], &buf, sizeof(buf));
        wait(0);
        printf("***pid:%d 进程结束***\n",getpid());
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
    }
    // 主进程
    close(data_fd[0]);
    int i ;
    for ( i = 2; i < 36; i++) {
        write(data_fd[1], &i, sizeof(int));
    }
    i = -1;
    write(data_fd[1], &i, sizeof(int));

    // TODO 无法退出程序，注释掉wait()可以但退出的时候明显子程序
    // 没执行结束，怀疑子程序退出不了  看不出来为什么退出不了?
    wait(0);
    exit(0);
}