/**
 * @file kdict.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 * 
 * Build:
 *  gcc -g -o test test.c kdict.c
 * Run:
 *  ./test
 *
 */
#ifndef __K_DICT_H__
#define __K_DICT_H__

typedef struct kdict_st
{
    int n;          /* Number of entries in dictionary */
    int size;       /* Storage size */
    char **val;     /* List of string values */
    char **key;     /* List of string keys */
    unsigned *hash; /* List of hash values for keys */
} kdict;

unsigned kdict_hash(char *key);

kdict *kdict_new(int size);
void kdict_free(kdict *d);

int kdict_set(kdict *d, char *key, char *val);
char *kdict_get(kdict *d, char *key, char *def);
void kdict_unset(kdict *d, char *key);

void kdict_output_file(kdict *d, FILE *out);

#endif