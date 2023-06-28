#ifndef __KPRIOQ_H__
#define __KPRIOQ_H__

#include <time.h>

typedef struct kprioq_elt_st
{
    time_t dt;
    unsigned long id;
} kprioq_elt;

typedef struct kprioq_st
{
    kprioq_elt *p;
    unsigned int len;
    unsigned int size;
} kprioq;

int kprioq_insert(kprioq *pq, kprioq_elt *pe);
int kprioq_min(kprioq *pq, kprioq_elt *pe);
void kprioq_delmin(kprioq *pq);

#endif