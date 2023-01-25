/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include "kmc.h"

int main(int argc, char **argv)
{
    int timeout = 3;
    int expire = 0;

    if (argc != 5) {
        printf("Usage: %s ip port key value\n", argv[0]);
        return 1;
    }

    char err[1024];

    // Set
    int n = kmc_set(argv[1], atoi(argv[2]), timeout, argv[3],
                    strlen(argv[3]), argv[4], strlen(argv[4]), expire, err, sizeof(err));
    if (n != 0) {
        fprintf(stderr, "set mc fail:%s\n", err);
        return 1;
    }
    printf("set mc succ\n");

    // Get
    int val_len = 0;
    char *val = kmc_get(argv[1], atoi(argv[2]), timeout, argv[3],
                        strlen(argv[3]), &val_len, err, sizeof(err));
    if (val == NULL) {
        fprintf(stderr, "get mc fail:[%d]%s", val_len, err);
        return 1;
    }
    printf("get mc succ: key(%s) -> %s\n", argv[3], val);
    free(val);

    // Del
    n = kmc_del(argv[1], atoi(argv[2]), timeout, argv[3],
                strlen(argv[3]), err, sizeof(err));
    if (n != 0) {
        fprintf(stderr, "delete mc fail:[%d]%s", n, err);
        return 1;
    }
    printf("delete mc succ\n");

    return 0;
}