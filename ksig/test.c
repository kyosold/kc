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
