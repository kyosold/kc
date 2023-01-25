/**
 * @file kurl.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __K_URL_H__
#define __K_URL_H__

#define KURL_LENGTH(len) (len * 3)

unsigned int kurl_encode(char const *s, int len, char *out, unsigned int out_size);
char *kurl_encode_alloc(char const *s, int len, int *ret_length);
int kurl_decode(char *str, int len, char *out, int out_size);


unsigned int kurl_raw_encode(char const *s, int len, char *out, unsigned int out_size);
char *kurl_raw_encode_alloc(char const *s, int len, int *ret_length);
int kurl_raw_decode(char *str, int len, char *out, unsigned int out_size);

#endif