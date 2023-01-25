/**
 * @file kfile.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-14
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __K_FILE_H__
#define __K_FILE_H__
#include <stdio.h>
#include "kqueue.h"
#include "../memwatch.h"

#define DEFAULT_DATA_BLOCK_SIZE 8888
typedef void *USER_ALLOCER(size_t);
typedef void USER_FREER(void *);

typedef struct data_block
{
    int data_len;
    char *data;
    int current_write_count;
    int current_read_count;
    TAILQ_ENTRY(data_block)
    entries;
} DATA_BLOCK;

TAILQ_HEAD(kfile_head, data_block);

/* kfile's handler */
typedef struct kfile
{
    int total_size;
    unsigned int new_header_size;
    struct kfile_head head; /* queue head */
    int block_num;          /* data block number */
    DATA_BLOCK *current_write_p;
    DATA_BLOCK *current_read_p;
    int is_full;  /* 0: not full, 1:full */
    int is_empty; /* already readed */
    int DATA_BLOCK_SIZE;
    int MAX_BLOCK_NUMBER;
    char ccache;
    USER_ALLOCER *alloc_fun;
    USER_FREER *free_fun;
} KFILE;

KFILE *kopen(int data_block_size, USER_ALLOCER *af, USER_FREER *fe);
void kclose(KFILE *handler);
int kwrite(KFILE *handler, const char *data, int len);
int kwrite_head(KFILE *handler, const char *data, int len);
int kread(KFILE *handler, char *data, int len);
void kseek(KFILE *handler);
void kseek_pos(KFILE *handler, int pos);
char kgetc(KFILE *handler);
int kread_line(KFILE *handler, char *buf, int n);
int kwrite_file(KFILE *handler, int fd);
int ksize(KFILE *handler);
unsigned int kfile_new_header_size(KFILE *handler);
int kfile_copy(KFILE *dest, KFILE *src, unsigned int start, unsigned int end);

#endif