/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <string.h>
#include "kdict.h"

#define NVALS 20000
#define DICT_INVALID_KEY ((char *)-1)

int main(void)
{
    kdict *d;
    char *val;
    int i;
    char cval[128];

    /* Allocate dictionary */
    printf("allocationg...\n");
    d = kdict_new(0);

    /* Set values in dictionary */
    printf("setting %d values...\n", NVALS);
    for (i = 0; i < NVALS; i++) {
        sprintf(cval, "%04d", i);
        kdict_set(d, cval, "salut");
    }
    printf("getting %d values...\n", NVALS);
    for (i = 0; i < NVALS; i++) {
        sprintf(cval, "%04d", i);
        val = kdict_get(d, cval, DICT_INVALID_KEY);
        if (val == DICT_INVALID_KEY) {
            printf("cannot get value for key [%s]\n", cval);
        }
    }
    printf("unsetting %d values...\n", NVALS);
    for (i = 0; i < NVALS; i++) {
        sprintf(cval, "%04d", i);
        kdict_unset(d, cval);
    }
    if (d->n != 0) {
        printf("error deleting values\n");
    }
    printf("deallocating...\n");
    kdict_free(d);

    return 0;
}