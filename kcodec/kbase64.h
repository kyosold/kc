/**
 * @file kbase64.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __K_BASE64_H__
#define __K_BASE64_H__

#define KBASE64_LENGTH(len) (((len + 2) / 3) * 4)

unsigned char *kbase64_encode_alloc(const unsigned char *str, int length, int *ret_length);

unsigned char *kbase64_decode_alloc(const unsigned char *str, int length, int *ret_length);

int kbase64_encode(const unsigned char *str, unsigned int str_len, char *out, unsigned int out_size);

int kbase64_decode(const unsigned char *str, unsigned int str_len, char *out, unsigned int out_size);

#endif