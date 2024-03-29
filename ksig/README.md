## 使用 `kill -l` 可以查看地系统定义的信号列表

## 信号的作用:
中断的系统调用: 如果进程在执行一个低速系统调用而阻塞期间捕捉到一个信号，则该系统调用就被中断不再继续执行。该系统调用返回出错是，其errno设置为EINTER。这样处理是因为一个信号发生了，进程捕捉到它，这意味着已经发生了某种事情，所以这是个好机会应当唤醒阻塞的系统调用。(这里必须区分`系统调用`和`函数`。当捕捉到某个信号时，被中断的是内核中执行的`系统调用`)

- 低速调用:
    - 如果某些类型文件的数据不存在(读管道、终端设备和网络设备)，则读操作可能会永远阻塞
    - 如果这些数据不能被相同的类型文件立即接受，则写操作可能会永远阻塞
    - 在某些条件发生之前打开某些类型文件，可能会发生阻塞（要打开一个终端设备，需要先等待与之连接的调制解答器应答）
    - pause函数和wait函数
    - 某些ioctl操作
    - 某些进程间通信函数

与被中断的系统调用相关的问题是必须显式地处理出错返回。典型的代码如下:
```c
again:
    if ((n = read(fd, buf, BUFFSIZE)) < 0) {
        if (errno == EINTR)
            goto again;     /* just an interrupted system call */
        /* handle other errors */
    }
```

## 可靠信号 --- 阻塞信号
当一个信号产生时，内核通常在进程表中以某种形式设置一个标志。当对信号采取了这种动作时，我们说向进程递送了一个信号。在信号产生和递送之间的时间间隔内，称信号是未决的(pending)。

进程可以选用”阻塞信号递送“。如果为进程产生一个阻塞的信号，而且对该信号的动作是系统默认或捕捉，则为该进程将此信号保持为`未决状态`，直到该进程对此信号解除了阻塞，或者将对此信号的动作更改为忽略。

- **如果进程解除对某个信号的阻塞之前，这种信号发生了多次，大多数UNIX并不对信号排队，而是只递送一次这种信号。（除非支持POSIX.1实时扩展）**
- **如果有多个信号要递送给一个进程，POSIX.1并没有规定这些信号的递送顺序。**

#### sigsuspend 使用:
- pause: 函数让进程直接进入阻塞状态，直到有信号到来

- 我们期待的情况:
   - sigprocmask(解除阻塞) ---> pause(等待新信号) ---> 信号到来
- 实际情况:
   - sigprocmask(解除阻塞) ---> 信号到来 ---> pause(等待新信号)
   - 如果之后不再有新信号到来的话，程序会一直阻塞在这里
- 解决的办法:
   - sigsuspend(解除阻塞 并 pause等待新信号) ---> 新信号到来
   - 将 解除阻塞 和 pause 合二为一

**例子说明:**
```c
#include <stdio.h>
#include <signal.h>
#include "ksig.h"

volatile sig_atomic_t quit_flag;

static void sig_int(int signo)
{
    if (signo == SIGINT)
        printf("interrupt\n");
    else if (signo == SIGQUIT)
        quit_flag = 1;  // set flag for main loop
}
int main(void)
{
    ksig_catch(SIGINT, sig_int);
    ksig_catch(SIGQUIT, sig_int);

    ksig_block(SIGQUIT);

    while (quit_flag == 0)
        ksig_pause();

    printf("byebye\n");
}
```
上面的例子是：设置一个全局变量quit_flag，该进程用于捕捉中断信号和退出信号，但是希望仅当捕捉到退出信号时，才退出。
- `ctrl+c`: 中断信号(SIGINT)
- `ctrl+z`: 挂起信号(SIGTSTP)
- `ctrl+\`: 退出信号(SIGQUIT)

#### 正确的使用阻塞信号方式:
```c
ksig_block(SIGCHLD);
ksig_catch(SIGCHLD, sigchld);

for (;;) {
    while (num_child >= limit)
        ksig_pause();   // 解除对信号的阻塞，并阻塞进程pause直接到新信号过来
    
    ksig_unblock(SIGCHLD);  // 解除对SIGCHLD信号的阻塞
    t = socket_accept4(...);
    ksig_block(SIGCHLD);
    if (t == -1) 
        continue;
    ++num_child;

    ...
}
```

    
