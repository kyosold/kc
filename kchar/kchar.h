/**
 * @file kchar.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-09
 *
 * @copyright Copyright (c) 2022
 *
 * Build:
 *  gcc -g -o test test.c kchar.c
 * Run:
 *  ./test
 * 
 */
#ifndef __K_CHAR_H__
#define __K_CHAR_H__

#define KCHAR_SUCC 0
#define KCHAR_FAIL 1

typedef struct kchar_st
{
    char *data;
    unsigned int len;
    unsigned int size;
} kchar;

kchar *kchar_new();
void kchar_clean(kchar *x);
void kchar_free(kchar *x);
int kchar_copy(kchar *dst, char *data, unsigned int n);
int kchar_cat(kchar *dst, char *data, unsigned int n);

#endif