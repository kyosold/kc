#include <stdlib.h>
#include <string.h>
#include "kchar.h"

#define ALIGNMENT   16  // assuming that this alignment is enough

static char *_alloc(unsigned int n)
{
    char *x = NULL;
    /**
     * (n & (ALIGNMENT - 1)): 为 n % 16
     * 如果 n%16 不为0，则向上加，使得n可以整除的16，
     * 使得申请的内存大小都是16字节的倍数(16,32,48,64...)
     */
    n = ALIGNMENT + n - (n & (ALIGNMENT - 1));
    x = (char *)malloc(n);
    return x;
}
static void _free(char *x)
{
    if (x) {
        free(x);
        x = NULL;
    }
}
static int _realloc(char **x, unsigned int old_size, unsigned int new_size)
{
    char *y = NULL;
    y = _alloc(new_size);
    if (!y)
        return KCHAR_FAIL;

    memcpy(y, *x, old_size);
    _free(*x);
    *x = y;

    return KCHAR_SUCC;
}

/**
 * @brief 检查x内存是否还可以容纳n个字节，
 * 如果内存不够则自动扩展，如果x->data为NULL，则自动malloc给它
 * 
 * @param x     
 * @param n     The number of bytes for check
 * @return int  Return KCHAR_SUCC or KCHAR_FAIL
 */
int kchar_ready(kchar *x, unsigned int n)
{
    unsigned int i;
    if (x->data) {
        n += x->len;
        if (n > x->size) {  // not enough
            i = 30 + n + (n >> 3);
            if (_realloc(&x->data, (x->size*sizeof(char)), (i*sizeof(char))) == KCHAR_FAIL) {
                return KCHAR_FAIL;
            }
            x->size = i;
            return KCHAR_SUCC;
        }
        return KCHAR_SUCC;
    }

    x->len = 0;
    x->size = n;
    x->data = _alloc(x->size);
    if (x->data == NULL)
        return KCHAR_FAIL;
    return KCHAR_SUCC;
}

/**
 * @brief Create a kchar *handle
 * 
 * @return kchar*   Return pointer to kchar handle or NULL is fail.
 * Use `kchar_free` to free
 */
kchar *kchar_new()
{
    kchar *x = (kchar *)_alloc(sizeof(kchar));
    if (x) {
        x->len = 0;
        x->size = 0;
        x->data = NULL;
    }
    return x;
}
/**
 * @brief Only clean resource data of x pointer to, 
 * Not include x, If you want free all, use kchar_free.
 * 
 * @param x clean data of x, x still exists.
 */
void kchar_clean(kchar *x)
{
    if (x) {
        if (x->data) {
            free(x->data);
        }
        x->len = 0;
        x->size = 0;
        x->data = NULL;
    }
}
/**
 * @brief free all resource of x, include x
 * 
 * @param x 
 */
void kchar_free(kchar *x)
{
    if (x) {
        if (x->data) {
            free(x->data);
            x->data = NULL;
        }
        free(x);
        x = NULL;
    }
}

/**
 * @brief   Copy n bytes from data to dst.
 * 
 * @param dst       pointer to kchar *handle
 * @param data      pointer to the object to copy from
 * @param n         number of bytes to copy
 * @return int      Return KCHAR_SUCC or KCHAR_FAIL
 */
int kchar_copy(kchar *dst, char *data, unsigned int n)
{
    kchar *x = dst;

    if (x == NULL) {
        return KCHAR_FAIL;
    }

    if (kchar_ready(x, n+1) == KCHAR_FAIL)
        return KCHAR_FAIL;

    memcpy(x->data, data, n);
    x->len = n;
    x->data[n] = '\0';      /* ``hahahaha`` */

    return KCHAR_SUCC;
}

/**
 * @brief   Appends a copy of n bytes from data to dst.
 * 
 * @param dst       pointer to kchar *handle
 * @param data      pointer to the object to copy from
 * @param n         number of bytes to copy
 * @return int      Return KCHAR_SUCC or KCHAR_FAIL
 */
int kchar_cat(kchar *dst, char *data, unsigned int n)
{
    if (dst == NULL)
        return KCHAR_FAIL;
    
    if (dst->data == NULL) {
        return kchar_copy(dst, data, n);
    }

    if (kchar_ready(dst, n + 1) == KCHAR_FAIL)
        return KCHAR_FAIL;

    memcpy(dst->data + dst->len, data, n);
    dst->len += n;
    dst->data[dst->len] = '\0';     /* ``hahahahaha`` */

    return KCHAR_SUCC;
}
