/**
 * @file kcrypto.h
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-25
 * 
 * @copyright Copyright (c) 2023
 * 
 * 书生成(命令行):
 *  1. 生成私钥:
 *      openssl genrsa -out private.pem 1024
 *  2. 根据私钥生成公钥:
 *      openssl rsa -in private.pem -out public.pem -pubout -outform PEM
 * 
 */
#ifndef __K_CRYPTO_H__
#define __K_CRYPTO_H__
#include <openssl/evp.h>

char *kcrypto_errstr();

//////////////////////////////
// Hash
//////////////////////////////
typedef enum
{
    KCRYPTO_MD4,
    KCRYPTO_MD5,
    KCRYPTO_SHA1,
    KCRYPTO_SHA224,
    KCRYPTO_SHA256,
    KCRYPTO_SHA384,
    KCRYPTO_SHA512
} KAlgorithm;
#define KCRYPTO_HASH_BUF_LEN    EVP_MAX_MD_SIZE * 4
unsigned int kcrypto_hash(KAlgorithm method, char *data, size_t data_len, char *out, size_t out_size);

//////////////////////////////
// Base64
//////////////////////////////
char *kcrypto_base64_encode_alloc(char *data, size_t data_len, size_t *out_len);
char *kcrypto_base64_decode_alloc(char *data, size_t data_len, size_t *out_len);

//////////////////////////////
// RSA
//////////////////////////////
typedef enum
{
    KCRYPTO_Invalid = 0,
    KCRYPTO_RSA_64 = 64,
    KCRYPTO_RSA_128 = 128,
    KCRYPTO_RSA_256 = 256,
    KCRYPTO_RSA_512 = 512,
    KCRYPTO_RSA_1024 = 1024,
    KCRYPTO_RSA_2048 = 2048,
    KCRYPTO_RSA_3072 = 3072,
    KCRYPTO_RSA_4096 = 4096,
    KCRYPTO_RSA_6144 = 6144,
    KCRYPTO_RSA_8192 = 8192
} KCRYPTO_RSA_Bits;

typedef struct kcrypto_keyparis {
    char *private_key_alloc;
    char *public_key_alloc;
} KCRYPTO_KEYPARIS;

struct kcrypto_keyparis *kcrypto_keypairs_alloc(KCRYPTO_RSA_Bits bits);
void kcrypto_keypairs_free(struct kcrypto_keyparis *keyparis);

char *kcrypto_public_encode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len);
char *kcrypto_private_decode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len);
char *kcrypto_private_encode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len);
char *kcrypto_public_decode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len);

//////////////////////////////
// AES
//////////////////////////////
char *kcrypto_aes_encode_alloc(char *passphrase, char *data, size_t data_len, size_t *out_len);
char *kcrypto_aes_decode_alloc(char *passphrase, char *data, size_t data_len, size_t *out_len);

#endif