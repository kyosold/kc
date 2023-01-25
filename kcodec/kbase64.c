/**
 * @file kbase64.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "kbase64.h"

// base64 tables
static const char base64_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

static const short base64_reverse_table[256] = {
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
    -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
    -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};

/**
 * @brief base64 encode
 * 
 * @param str           The data to encode
 * @param length        The length of data
 * @param ret_length    The length of return result
 * @return unsigned char* The encoded data, as a string.
 */
unsigned char *kbase64_encode_alloc(const unsigned char *str, int length, int *ret_length)
{
    unsigned char *result = NULL;
    unsigned int result_size = ((length + 2) / 3) * (4 * sizeof(char));

    if (length < 0) {
        if (ret_length != NULL) 
            *ret_length = 0;
        return result;
    }

    if (((size_t)length + 2) / 3 > INT_MAX/4) {
        // String too long, maximum is INT_MAX/4
        if (ret_length != NULL)
            *ret_length = 0;
        return result;
    }

    result = (unsigned char *)calloc((length + 2) / 3, 4 * sizeof(char));
    if (result == NULL) {
        if (ret_length != NULL)
            *ret_length = 0;
        return result;
    }

    int n = kbase64_encode(str, length, result, result_size);
    if (ret_length)
        *ret_length = n;

    return result;
}

/**
 * @brief Decodes data encoded with MIME base64
 * 
 * @param str           The encoded data.
 * @param length        The length of data
 * @param ret_length    The length of return result
 * @return unsigned char* Returns the decoded data or NULL on failure. The returned data may be binary.
 */
unsigned char *kbase64_decode_alloc(const unsigned char *str, int length, int *ret_length)
{
    unsigned char *result;
    unsigned int result_size = length;

    result = (unsigned char *)calloc(length, 1);
    if (result == NULL) {
        if (ret_length != NULL)
            *ret_length = 0;
        return NULL;
    }

    int n = kbase64_decode(str, length, result, result_size);
    if (ret_length)
        *ret_length = n;

    return result;
}

int kbase64_encode(const unsigned char *str, unsigned int str_len, char *out, unsigned int out_size)
{
    const unsigned char *current = str;
    unsigned char *p;
    unsigned char *result = out;

    if (str_len < 0) {
        return 0;
    }
    if (out_size < KBASE64_LENGTH(str_len)) {
        return 0;
    }
    p = result;

    while (str_len > 2)  // keep going until we have less than 24 bits
    {
        *p++ = base64_table[current[0] >> 2];
        *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        *p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        *p++ = base64_table[current[2] & 0x3f];

        current += 3;
        str_len -= 3;    // we just handle 3 octets of data
    }

    // now deal with the tail end of things
    if (str_len != 0) {
        *p++ = base64_table[current[0] >> 2];
        if (str_len > 1) {
            *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            *p++ = base64_table[(current[1] & 0x0f) << 2];
            *p++ = base64_pad;
        } else {
            *p++ = base64_table[(current[0] & 0x03) << 4];
            *p++ = base64_pad;
            *p++ = base64_pad;
        }
    }

    *p = '\0';
    return (int)(p - result);
}

int kbase64_decode(const unsigned char *str, unsigned int str_len, char *out, unsigned int out_size)
{
    const unsigned char *current = str;
    int ch, i = 0, j = 0, k;
    // this sucks for threaded environments
    unsigned char *result = out;

    // 如果strict参数被设置为1，那么如果输入包含base64字母表以外的字符，base64_decode()函数将返回false。否则无效字符将被无声地丢弃。
    int strict = 0;

    // run through the whole string, converting as we go
    while ((ch = *current++) != '\0' && str_len-- > 0) {
        if (ch == base64_pad) {
            if (*current != '=' && ((i % 4) == 1 || (strict && str_len > 0))) {
                if ((i % 4) != 1) {
                    while (isspace(*(++current))) {
                        continue;
                    }
                    if (*current == '\0') {
                        continue;
                    }
                }
                out[0] = '\0';
                return 0;
            }
            continue;
        }

        ch = base64_reverse_table[ch];
        if ((!strict && ch < 0) || ch == -1) {  // a space or some other separator character, we simply skip over
            continue;
        } else if (ch == -2) {
            out[0] = '\0';
            return 0;
        }

        switch (i % 4)
        {
        case 0:
            result[j] = ch << 2;
            break;
        case 1:
            result[j++] |= ch >> 4;
            result[j] = (ch & 0x0f) << 4;
            break;
        case 2:
            result[j++] |= ch >>2;
            result[j] = (ch & 0x03) << 6;
            break;
        case 3:
            result[j++] |= ch;
            break;
        }
        i++;
    }

    k = j;
    // mop things up if we ended on a boundary
    if (ch == base64_pad) {
        switch (i % 4)
        {
        case 1:
            out[0] = '\0';
            return 0;
        case 2:
            k++;
        case 3:
            result[k] = 0;
        }
    }

    result[j] = '\0';
    return j;
}
