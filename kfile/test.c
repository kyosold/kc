/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 * Build:
 *  gcc -g -o test test.c kfile.c
 * Run:
 *  ./test abc.eml
 * 
 */
#include <stdio.h>
#include <string.h>
#include "kfile.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char *file = argv[1];
    unsigned int size = 24;
    char buf[1024] = {0};
    size_t n = 0, m = 0;

    FILE *fp = fopen(file, "r");
    if (!fp) {
        fprintf(stderr, "open file(%s) failed\n", file);
        return 1;
    }

    KFILE *kfp = kopen(128, NULL, NULL);    // 每个内存块大小为1024
    if (kfp == NULL) {
        fprintf(stderr, "kopen fail\n");
        return 1;
    }
    while (!feof(fp)) {
        n = fread(buf, 1, size, fp);
        if (kwrite(kfp, buf, n) == -1) {
            fprintf(stderr, "kwrite failed\n");
            break;
        }
        m += n;
        // buf[n] = 0;      注意：如果输出要加\0
        // printf("%s", buf);
    }
    fclose(fp);
    fprintf(stderr, "Finished read file:%d\n", m);

    printf("save file size:%d\n\n", ksize(kfp));

    kseek(kfp);

    for (;;) {
        n = kread_line(kfp, buf, sizeof(buf) - 1);
        if (n)
        {
            buf[n] = 0;
            printf("%s", buf);
        }
        else
        {
            break;
        }
    }

    kclose(kfp);
    

    return 0;
}