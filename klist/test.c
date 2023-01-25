/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * Build:
 *  gcc -g -o test test.c klist.c
 * Run:
 *  ./test
 * 
 */
#include <stdio.h>
#include "klist.h"

int main(void)
{
    klist *list = NULL; // 注意这里的NULL，必须，必须，必须

    list = klist_append(list, "name: kyosold");
    list = klist_append(list, "age: 33");
    list = klist_append(list, "email: kyosold@qq.com");

    // show
    int i = 0;
    klist *next = NULL;
    klist *item = list;
    do {
        printf("[%d] %s\n", i++, item->data);

        next = item->next;
        item = next;
    } while (next);

    klist_free_all(list);

    return;
}