/**
 * @file kurl.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include "kurl.h"

static unsigned char hexchars[] = "0123456789ABCDEF";

static int kurl_htoi(char *s)
{
    int value;
    int c;

    c = ((unsigned char *)s)[0];
    if (isupper(c))
        c = tolower(c);
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((unsigned char *)s)[1];
    if (isupper(c))
        c = tolower(c);
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}

char *kurl_encode_alloc(char const *s, int len, int *ret_length)
{
    register unsigned char c;
    unsigned char *result;
    unsigned int result_size;
    unsigned char const *from, *end;

    result = (unsigned char *)calloc(3, len);
    if (result == NULL) {
        if (ret_length)
            *ret_length = 0;
        return NULL;
    }
    result_size = 3 * len;
    unsigned int n = kurl_encode(s, len, result, result_size);

    if (n > INT_MAX) {
        if (ret_length)
            *ret_length = 0;
        free(result);
        return NULL;
    }

    if (ret_length)
        *ret_length = n;

    return (char *)result;
}

/**
 * @brief URL-encodes string. (' '->'+')
 * 
 * @param s                 The string to be encoded
 * @param len               The length of s
 * @param out               The store result
 * @param out_size          The size of out, the size: (3*len)
 * @return unsigned int     Return the length of out
 */
unsigned int kurl_encode(char const *s, int len, char *out, unsigned int out_size)
{
    if (out_size < (3 * len))
        return 0;

    register unsigned char c;
    unsigned char *to, *start;
    unsigned char const *from, *end;

    from = (unsigned char *)s;
    end = (unsigned char *)s + len;
    start = to = out;

    while (from < end) {
        c = *from++;

        if (c == ' ') {
            *to++ = '+';
        } else if ((c < '0' && c != '-' && c != '.') ||
                    (c < 'A' && c > '9') ||
                    (c > 'Z' && c < 'a' && c != '_') ||
                    (c > 'z')) {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
        } else {
            *to++ = c;
        }
    }

    *to = 0;
    return (to - start);
}

/**
 * @brief Decodes URL-encoded string
 * 
 * @param str           The string to be decoded.
 * @param len           The length of str
 * @param out           The store result
 * @param out_size      The size of out, the size: len
 * @return int          Return the length of out.
 */
int kurl_decode(char *str, int len, char *out, int out_size)
{
    if (out_size < len)
        return 0;

    char *dest = out;
    char *data = str;

    while (len--) {
        if (*data == '+') {
            *dest = ' ';
        } else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1))
                     && isxdigit((int) *(data + 2))) {

            *dest = (char)kurl_htoi(data + 1);

            data += 2;
            len -= 2;
        } else {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return dest - out;
}

char *kurl_raw_encode_alloc(char const *s, int len, int *ret_length)
{
    unsigned char *str;
    unsigned int str_size = 3 * len;

    str = (unsigned char *)calloc(3, len);
    if (str == NULL) {
        if (ret_length)
            *ret_length = 0;
        return NULL;
    }

    int n = kurl_raw_encode(s, len, str, str_size);
    if (n == 0) {
        free(str);
        str = NULL;
    }

    if (ret_length)
        *ret_length = n;

    return str;
}

/**
 * @brief URL-encodes string to RFC 3986. (' '->'%20')
 * 
 * @param s                 The string to be encoded
 * @param len               The length of s
 * @param out               The store result
 * @param out_size          The size of out, the size: (3*len)
 * @return unsigned int     Return the length of out
 */
unsigned int kurl_raw_encode(char const *s, int len, char *out, unsigned int out_size)
{
    register size_t x, y;
    unsigned char *str = out;

    if (out_size < (3 * len))
        return 0;

    for (x = 0, y = 0; len--; x++, y++) {
        str[y] = (unsigned char) s[x];
        if ((str[y] < '0' && str[y] != '-' && str[y] != '.') ||
            (str[y] < 'A' && str[y] > '9') ||
            (str[y] > 'Z' && str[y] < 'a' && str[y] != '_') ||
            (str[y] > 'z' && str[y] != '~')) {
            str[y++] = '%';
            str[y++] = hexchars[(unsigned char) s[x] >> 4];
            str[y] = hexchars[(unsigned char) s[x] & 15];
        }
    }
    str[y] = '\0';
    if (y > INT_MAX) {
        return 0;
    }
    return y;
}

/**
 * @brief Decodes URL-encoded string
 * 
 * @param str           The string to be decoded.
 * @param len           The length of str
 * @param out           The store result
 * @param out_size      The size of out, the size: len
 * @return int          Return the length of out.
 */
int kurl_raw_decode(char *str, int len, char *out, unsigned int out_size)
{
    if (out_size < len)
        return 0;

    char *dest = out;
    char *data = str;

    while (len--) {
        if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1))
            && isxdigit((int) *(data + 2))) {
            *dest = (char)kurl_htoi(data + 1);

            data += 2;
            len -= 2;
        } else {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return (dest - out);
}
