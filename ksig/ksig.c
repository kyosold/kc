#include <signal.h>
#include "ksig.h"

void ksig_catch(int sig, void (*f)())
{
    struct sigaction sa;
    sa.sa_handler = f;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, (struct sigaction *) 0);
}
void ksig_block(int sig)
{
    sigset_t ss; 
    sigemptyset(&ss);
    sigaddset(&ss, sig);
    sigprocmask(SIG_BLOCK, &ss, (sigset_t *) 0); 
}
void ksig_unblock(int sig)
{
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, sig);
    sigprocmask(SIG_UNBLOCK, &ss, (sigset_t *) 0);
}
void ksig_blocknone()
{
    sigset_t ss;
    sigemptyset(&ss);
    sigprocmask(SIG_SETMASK, &ss, (sigset_t *) 0);
}
// 先恢复信号，然后使进程休眠直到有信号产生
void ksig_pause()
{
    sigset_t ss;
    sigemptyset(&ss);
    sigsuspend(&ss);
}

void ksig_miscignore()
{
    ksig_catch(SIGVTALRM, SIG_IGN);
    ksig_catch(SIGPROF, SIG_IGN);
    ksig_catch(SIGQUIT, SIG_IGN);
    ksig_catch(SIGINT, SIG_IGN);
    ksig_catch(SIGHUP, SIG_IGN);
#ifdef SIGXCPU
    ksig_catch(SIGXCPU, SIG_IGN);
#endif
#ifdef SIGXFSZ
    ksig_catch(SIGXFSZ, SIG_IGN);
#endif
}
void ksig_bugcatch(void (*f)())
{
    ksig_catch(SIGILL, f);
    ksig_catch(SIGABRT, f);
    ksig_catch(SIGFPE, f);
    ksig_catch(SIGBUS, f);
    ksig_catch(SIGSEGV, f);
#ifdef SIGSYS
    ksig_catch(SIGSYS, f);
#endif
#ifdef SIGEMT
    ksig_catch(SIGEMT, f);
#endif
}

/****** SIGPIPE: 如果在管道的读进程已经终止时写该管道 ******/
void ksig_pipeignore()
{
    ksig_catch(SIGPIPE, SIG_IGN);
}
void ksig_pipedefault()
{
    ksig_catch(SIGPIPE, SIG_DFL);
}

/****** SIGTERM: 终止 ******/
void ksig_termblock()
{
    ksig_block(SIGTERM);
}
void ksig_termunblock()
{
    ksig_unblock(SIGTERM);
}
void ksig_termcatch(void (*f)())
{
    ksig_catch(SIGTERM, f);
}
void ksig_termdefault()
{
    ksig_catch(SIGTERM, SIG_DFL);
}

/****** SIGALRM: 定时器超时 ******/
void ksig_alarmblock()
{
    ksig_block(SIGALRM);
}
void ksig_alarmunblock()
{
    ksig_unblock(SIGALRM);
}
void ksig_alarmcatch(void (*f)())
{
    ksig_catch(SIGALRM, f);
}
void ksig_alarmdefault()
{
    ksig_catch(SIGALRM, SIG_DFL);
}

/****** SIGCHLD: 子进程状态改变 ******/
void ksig_childblock()
{
    ksig_block(SIGCHLD);
}
void ksig_childunblock()
{
    ksig_unblock(SIGCHLD);
}
void ksig_childcatch(void (*f)())
{
    ksig_catch(SIGCHLD, f);
}
void ksig_childdefault()
{
    ksig_catch(SIGCHLD, SIG_DFL);
}

/****** SIGHUP: 连接断开 ******/
void ksig_hangupblock()
{
    ksig_block(SIGHUP);
}
void ksig_hangupunblock()
{
    ksig_unblock(SIGHUP);
}
void ksig_hangupcatch(void (*f)())
{
    ksig_catch(SIGHUP, f);
}
void ksig_hangupdefault()
{
    ksig_catch(SIGHUP, SIG_DFL);
}