/**
 * @file kfile.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include "kfile.h"

/**
 * @brief 
 * 
 * @param data_block_size   block size
 * @param af        User custom alloc function
 * @param fe        user custom free function
 * @return KFILE*   Return a KFILE pointer. Otherwise, NULL is returned
 */
KFILE *kopen(int data_block_size, USER_ALLOCER *af, USER_FREER *fe)
{
    KFILE *handler;

    if (af == NULL && fe == NULL) {
        handler = malloc(sizeof(KFILE));
        memset(handler, 0, sizeof(KFILE));
        handler->alloc_fun = malloc;
        handler->free_fun = free;
    } else if (af != NULL && fe != NULL) {
        handler = af(sizeof(KFILE));
        memset(handler, 0, sizeof(KFILE));
        handler->free_fun = fe;
        handler->alloc_fun = af;
    } else {
        return NULL;
    }

    if (handler == NULL) {
        return NULL;    /* malloc error in mopen */
    }
#ifdef MALLOC_DEBUG
    ++total_malloc;
#endif
    TAILQ_INIT(&handler->head);
    handler->is_full = 1;
    handler->is_empty = 0;
    handler->ccache = '\n';
    if (data_block_size == 0) {
        handler->DATA_BLOCK_SIZE = DEFAULT_DATA_BLOCK_SIZE;
    } else {
        handler->DATA_BLOCK_SIZE = data_block_size;
    }
    return handler;
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 */
void kclose(KFILE *handler)
{
    DATA_BLOCK *n1;
    DATA_BLOCK *n2;

    if (NULL == handler)
        return;
    n1 = TAILQ_FIRST(&handler->head);
    while (n1 != NULL) {
        n2 = TAILQ_NEXT(n1, entries);
        handler->free_fun(n1->data);
        handler->free_fun(n1);
#ifdef MALLOC_DEBUG
        total_free += 2;
#endif
        n1 = n2;
    }
    free(handler);
#ifdef MALLOC_DEBUG
    ++total_free;
#endif
}

/**
 * @brief 
 * 
 * @param handler 
 */
void kseek(KFILE *handler)
{
    DATA_BLOCK *n1;
    DATA_BLOCK *n2;

    if (NULL == handler)
        return;
    handler->is_empty = 0;
    handler->current_read_p = NULL;
    n1 = TAILQ_FIRST(&handler->head);
    while (n1 != NULL) {
        n1->current_read_count = 0;
        n2 = TAILQ_NEXT(n1, entries);
        n1 = n2;
    }
}

void kseek_pos(KFILE *handler, int pos)
{
    pos = pos < 0 ? 0 : pos;
    kseek(handler);
    DATA_BLOCK *n1;
    n1 = TAILQ_FIRST(&handler->head);
    while (n1 != NULL) {
        if (pos <= n1->current_write_count) {
            n1->current_read_count = pos;
            handler->current_read_p = n1;
            break;
        }
        pos -= n1->current_write_count;
        n1 = TAILQ_NEXT(n1, entries);
    }
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @param data      the data you want save
 * @param len       length of data
 * @return int      Upon successful completion return 1.
 * Otherwise, -1 is returned
 */
int kwrite(KFILE *handler, const char *data, int len)
{
    DATA_BLOCK *new_block;
    DATA_BLOCK *current_block;
    int free_count;
    int writed;
    char *curr_p;

    if (NULL == handler)
        return -1;

    writed = 0;
    handler->total_size += len;
    while (writed < len) {
        if (handler->is_full) {
            new_block = handler->alloc_fun(sizeof(DATA_BLOCK));
            if (new_block == NULL) {
                return -1;  /* alloc_fun data_block error */
            }
#ifdef MALLOC_DEBUG
++total_malloc;
#endif
            memset(new_block, 0, sizeof(DATA_BLOCK));
            new_block->data = (char *)handler->alloc_fun(handler->DATA_BLOCK_SIZE);
            if (new_block->data == NULL) {
                handler->free_fun(new_block);   /* alloc_fun data buffer error */
                return -1;
            }
#ifdef MALLOC_DEBUG
++total_malloc;
#endif
            handler->block_num++;
            handler->current_write_p = new_block;
            TAILQ_INSERT_TAIL(&handler->head, new_block, entries);
        }
        current_block = handler->current_write_p;
        free_count = handler->DATA_BLOCK_SIZE - (current_block->current_write_count);
        curr_p = current_block->data + (current_block->current_write_count);
        if ((len - writed) <= free_count) {
            memcpy(curr_p, data + writed, len - writed);
            current_block->current_write_count += (len - writed);
            current_block->data_len = current_block->current_write_count;
            if (current_block->current_write_count == handler->DATA_BLOCK_SIZE) {
                handler->is_full = 1;
            } else {
                handler->is_full = 0;
            }
            break;
        } else {
            memcpy(curr_p, data + writed, free_count);
            current_block->current_write_count += free_count;
            current_block->data_len = current_block->current_write_count;
            writed += free_count;
            handler->is_full = 1;
        }
    }
    return 1;
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @param data      the data you want save
 * @param len       length of data
 * @return int      Upon successful completion return 1.
 *  Otherwise, -1 is returned
 */
int kwrite_head(KFILE *handler, const char *data, int len)
{
    DATA_BLOCK *new_block;
    char *curr_p;
                
    if (NULL == handler)
        return -1;
    if (len > handler->DATA_BLOCK_SIZE) {
        return -1;  /* data too long */
    }
    handler->total_size += len;
    new_block = handler->alloc_fun(sizeof(DATA_BLOCK));
    if (NULL == new_block) {
        return -1;  /* alloc_fun data_block error */
    }
    memset(new_block, 0, sizeof(DATA_BLOCK));
    new_block->data = (char *)handler->alloc_fun(handler->DATA_BLOCK_SIZE);
    if (NULL == new_block->data) {
        handler->free_fun(new_block);   /* alloc_fun data buffer error */
        return -1;
    }
    handler->new_header_size += len;
    handler->block_num++;
    TAILQ_INSERT_HEAD(&handler->head, new_block, entries);
    curr_p = new_block->data;
    new_block->current_write_count = len;
    new_block->data_len = len;
    memcpy(curr_p, data, len);
    return 1;
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @param data      the data you want read
 * @param len       length of data buffer
 * @return int      Upon successful completion return size that you readed.
 * in the end, -1 is returned
 */
int kread(KFILE *handler, char *data, int len)
{
    DATA_BLOCK *new_block;
    DATA_BLOCK *current_block;
    int could_read;
    char *curr_p;

    if (NULL == handler)
        return -1;

    if (NULL == handler->current_read_p) {
        handler->current_read_p = TAILQ_FIRST(&handler->head);
        if (NULL == handler->current_read_p) {
            return -1;
        }
    }

    int readed = 0;
    while (readed < len) {
        if (handler->is_empty) {
            new_block = TAILQ_NEXT(handler->current_read_p, entries);
            if (NULL == new_block) {
                break;
            }
            handler->current_read_p = new_block;
        }

        current_block = handler->current_read_p;
        could_read = current_block->current_write_count - current_block->current_read_count;
        if (could_read <= 0) {
            break;
        }

        curr_p = current_block->data + current_block->current_read_count;
        if ((len - readed) <= could_read) {
            memcpy(data + readed, curr_p, (len - readed));
            current_block->current_read_count += (len - readed);
            readed += (len - readed);
            if (current_block->current_read_count == current_block->current_write_count) {
                handler->is_empty = 1;
            } else {
                handler->is_empty = 0;
            }
            break;
        } else {
            memcpy(data + readed, curr_p, could_read);
            current_block->current_read_count += could_read;
            readed += could_read;
            handler->is_empty = 1;
        }
    }

    return readed;
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @return char     Upon successful completion return char that you readed.
 * in the end, 0 is returned
 */
char kgetc(KFILE *handler)
{
    DATA_BLOCK *new_block;
    DATA_BLOCK *current_block;
    char *curr_p;

    if (NULL == handler)
        return 0;
    if (NULL == handler->current_read_p) {
        handler->current_read_p = TAILQ_FIRST(&handler->head);
        if (handler->current_read_p == NULL) {
            return 0;
        }
    }
    if (handler->is_empty) {
        //next:
        new_block = TAILQ_NEXT(handler->current_read_p, entries);
        if (NULL == new_block) {
            return 0;
        }
        handler->current_read_p = new_block;
    }
    current_block = handler->current_read_p;
    curr_p = current_block->data + current_block->current_read_count;
    current_block->current_read_count++;
    if (current_block->current_read_count
        == current_block->current_write_count) {
            handler->is_empty = 1;
    } else {
        handler->is_empty = 0;
    }
    return *curr_p;
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @param buf       the data you want read
 * @param n         length of data buffer
 * @return int      Upon successful completion return size that you readed.
 * in the end, 0 is returned
 */
int kread_line(KFILE *handler, char *buf, int n)
{
    int r;

    if (handler == NULL)
        return 0;
    r = 0;
    buf[n-1] = '\0';
    n -= 2;
    if (n != 0) {
        char *d = buf;
        do {
            if ('\n' == handler->ccache) {
                *d = kgetc(handler);
            } else {
                *d = handler->ccache;
                handler->ccache = '\r';
            }   
            if (*d == '\r') {
                handler->ccache = kgetc(handler);
                if ( '\0' == handler->ccache ) { 
                    *(++d) = '\n';
                    *(++d) = '\0';
                    r = d - buf;
                    break;
                }   
                else if ('\n' != handler->ccache) {
                    d++;
                    r = d - buf;
                    break;
                }
                else {
                    d++;
                    *d = handler->ccache;
                    d++;
                    r = d - buf;
                    break;
                }
            } else if (*d == '\n') {
                d++;
                r = d - buf;
                break;
            } else if (*d == 0) {
                r = d - buf;
                break;
            }
            d++;
        } while (--n > 0);
    }

    /* return the position that the string ends. */
    return (r);
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @param fd 
 * @return int      Upon successful completion the number of bytes
 * which were written is returned.
 * Otherwise, -1 is returned
 */
int kwrite_file(KFILE *handler, int fd)
{
    const int MAX_BLOCK_NUMBER = 1024;
    struct iovec iov[MAX_BLOCK_NUMBER];
    int n_iov;
    DATA_BLOCK *dbnp;
    int file_size;
    int retval;

    if (NULL == handler)
        return -1;
    
    n_iov = 0;
    dbnp = NULL;
    file_size = 0;
    TAILQ_FOREACH(dbnp, &(handler->head), entries) {
        iov[n_iov].iov_base = dbnp->data;
        iov[n_iov].iov_len = dbnp->data_len < 0 ? 0 : dbnp->data_len;
        n_iov++;
        file_size += dbnp->data_len;
        if (n_iov >= MAX_BLOCK_NUMBER)
            return -1;
    }

    if (file_size == 0)
        return 0;
    
    retval = writev(fd, iov, n_iov);
    if (retval < 0) {
        return -1;
    } else {
        return retval;
    }
}

/**
 * @brief 
 * 
 * @param handler   handler of savadata
 * @return int      Upon successful completion the number of bytes
 * which were written is returned.
 * Otherwise, -1 is returned
 */
int ksize(KFILE *handler)
{
    return handler->total_size;
}

/**
 * @brief 
 * 
 * @param handler 
 * @return unsigned int     Upon successful completion the number of 
 * bytes which were written head is returned.
 */
unsigned int kfile_new_header_size(KFILE *handler)
{
    return handler->new_header_size;
}

/**
 * @brief 
 * 
 * @param dest      pointer to the destination of KFILE to copy to
 * @param src       pointer to the srouce of KFILE to copy from
 * @param start     start position index
 * @param end       end position index
 * @return int      return the number of bytes copied
 */
int kfile_copy(KFILE *dest, KFILE *src, unsigned int start, unsigned int end)
{
    char buffer[1024];
    kseek_pos(src, start);

    unsigned int i = start;
    unsigned int j = 0;
    int ret;
    while (i < end) {
        j = end - i;
        j = j < 1024 ? j : 1024;
        ret = kread(src, buffer, j);
        if (ret <= 0)
            return -1;
        else {
            kwrite(dest, buffer, ret);
            if (ret < 1024)
                break;
        }
        i += j;
    }
    return end - start;
}