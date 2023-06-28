#include <stdlib.h>
#include <string.h>
#include "kprioq.h"

/**
 * @brief 
 * 
 * @param x 
 * @param n 
 * @return int 0:succ, 1:fail
 */
int kprioq_ready(kprioq *x, unsigned int n)
{
    unsigned int i;
    if (x->p) {
        i = x->size;
        n += x->len;
        if (n > i) {
            x->size = 100 + n + (n >> 3);
            kprioq_elt *new_ptr = realloc(x->p, x->size + sizeof(kprioq_elt));
            if (new_ptr != NULL) {
                x->p = new_ptr;
                return 0;
            }
            x->size = i;
            return 1;
        }
        return 0;
    }
    x->len = 0;
    x->size = n;
    x->p = (kprioq_elt *)malloc(x->size * sizeof(kprioq_elt));
    if (x->p != NULL)
        return 0;
    else
        return 1;
}

/**
 * @brief 
 * 
 * @param pq 
 * @param pe 
 * @return int 0:succ, 1:fail
 */
int kprioq_insert(kprioq *pq, kprioq_elt *pe)
{
    int i;
    int j;
    if (kprioq_ready(pq, 1))
        return 1;
    j = pq->len++;
    while (j) {
        i = (j - 1) / 2;
        if (pq->p[i].dt <= pe->dt)
            break;
        pq->p[j] = pq->p[i];
        j = i;
    }
    pq->p[j] = *pe;
    return 0;
}
int kprioq_min(kprioq *pq, kprioq_elt *pe)
{
    if (!pq->p)
        return 1;
    if (!pq->len)
        return 1;
    *pe = pq->p[0];
    return 0;
}
void kprioq_delmin(kprioq *pq)
{
    int i;
    int j;
    int n;
    if (!pq->p)
        return;
    n = pq->len;
    if (!n)
        return;
    i = 0;
    --n;
    for (;;) {
        j = i + i + 2;
        if (j > n)
            break;
        if (pq->p[j-1].dt <= pq->p[j].dt)
            --j;
        if (pq->p[n].dt <= pq->p[j].dt)
            break;
        pq->p[i] = pq->p[j];
        i = j;
    }
    pq->p[i] = pq->p[n];
    pq->len = n;
}