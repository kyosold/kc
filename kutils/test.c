/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-20
 * 
 * @copyright Copyright (c) 2023
 * 
 * Build:
 *  gcc -g -o test test.c kutils.c
 * Run:
 *  ./test
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kutils.h"

int main(int argc, char **argv)
{
    // --- ktrim_str ----
    char str[] = "  hello, world!  \n";
    char result[1024] = {0};

    ktrim_str(str, strlen(str), NULL, 3, result, sizeof(result));
    printf("%ld:%s\n", strlen(result), result);

    // --- ksystem_alloc ---
    strcpy(str, "ls -l");
    char *res_alloc = ksystem_alloc(str);
    printf("%s\n", res_alloc);
    free(res_alloc);

    // --- ksystem ---
    char res[4096] = {0};
    int n = ksystem(str, res, sizeof(res));
    printf("%d\n", n);
    printf("%s\n", res);
    

    return 0;
}