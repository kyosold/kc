/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <string.h>
#include "kchar.h"

int main(void)
{
    kchar *s = NULL;

    s = kchar_new();
    if (!s) {
        printf("init kchar fail\n");
        return 1;
    }

    // copy and append
    kchar_copy(s, "hello:", strlen("hello:"));
    kchar_cat(s, "\r\n", 2);
    kchar_cat(s, "  kyosold, this is test.\n", strlen("  kyosold, this is test.\n"));

    // show
    printf("%s", s->data);

    // clean
    kchar_free(s);

    return 0;
}