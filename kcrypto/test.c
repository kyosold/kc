/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 * Depend:
 *  openssl-devel-1.1.1m
 * 
 * Build:
 *  gcc -g -o test test.c kcrypto.c -I/usr/local/openssl-1.1.1m/include/ -L/usr/local/openssl-1.1.1m/lib/ -lcrypto -lssl
 * 
 * Run:
 *  ./test
 * 
 * 
 * 
 */
#include <stdio.h>
#include <string.h>
#include "kcrypto.h"

int main(int argc, char **argv)
{
    // if (argc != 2)
    //     return 1;

    // --- Hash ---
    char res[KCRYPTO_HASH_BUF_LEN];
    int n = kcrypto_hash(KCRYPTO_MD5, argv[1], strlen(argv[1]), res, sizeof(res));
    printf("KCRYPTO_MD5: [%d]%s\n", n, res);

    n = kcrypto_hash(KCRYPTO_SHA512, argv[1], strlen(argv[1]), res, sizeof(res));
    printf("KCRYPTO_SHA512: [%d]%s\n", n, res);

    // --- base64 ---
    size_t b64_len = 0;
    char *b64 = kcrypto_base64_encode_alloc(argv[1], strlen(argv[1]), &b64_len);
    if (b64) {
        printf("base64 encode: [%d]%s\n", b64_len, b64);
    }
    size_t b64_dec_len = 0;
    char *b64_dec = kcrypto_base64_decode_alloc(b64, strlen(b64), &b64_dec_len);
    if (b64_dec) {
        printf("base64 decode: [%d]%s\n", b64_dec_len, b64_dec);
    }
    if (b64)
        free(b64);
    if (b64_dec)
        free(b64_dec);


    // --- gen keypairs ---
    struct kcrypto_keyparis *kp = kcrypto_keypairs_alloc(KCRYPTO_RSA_1024);
    if (kp) {
        printf("%s\n", kp->private_key_alloc);
        printf("%s\n", kp->public_key_alloc);
    }

    // --- public -> private ---
    printf("argc: %d\n", argc);
    size_t encrypted_len = 0;
    char *encrypted_data = kcrypto_public_encode_alloc(kp->public_key_alloc,
                                                       argv[1],
                                                       strlen(argv[1]),
                                                       &encrypted_len);
    if (encrypted_data) {
        printf("public key encrypted:[%d]%s\n\n", encrypted_len, encrypted_data);
    }

    size_t decrypted_len = 0;
    char *decrypted_data = kcrypto_private_decode_alloc(kp->private_key_alloc,
                                                        encrypted_data,
                                                        encrypted_len,
                                                        &decrypted_len);
    if (decrypted_data) {
        printf("private key decrypted:[%d]%s\n\n", decrypted_len, decrypted_data);
    }

    if (encrypted_data)
        free(encrypted_data);
    if (decrypted_data)
        free(decrypted_data);

    // --- private -> public ---
    encrypted_data = kcrypto_private_encode_alloc(kp->private_key_alloc,
                                                  argv[1],
                                                  strlen(argv[1]),
                                                  &encrypted_len);
    if (encrypted_data) {
        printf("private key encrypted:[%d]%s\n\n", encrypted_len, encrypted_data);
    }

    decrypted_data = kcrypto_public_decode_alloc(kp->public_key_alloc,
                                                 encrypted_data,
                                                 encrypted_len,
                                                 &decrypted_len);
    if (!decrypted_data) {
        printf("Err: %s\n", kcrypto_errstr);
    }
    printf("public key decrypted:[%d]%s\n\n", decrypted_len, decrypted_data);
    if (encrypted_data)
        free(encrypted_data);
    if (decrypted_data)
        free(decrypted_data);

    kcrypto_keypairs_free(kp);


    // --- aes ---
    size_t aes_encrypted_len = 0;
    char *aes_encrypted = kcrypto_aes_encode_alloc("13456",
                                                   argv[1],
                                                   strlen(argv[1]),
                                                   &aes_encrypted_len);
    if (aes_encrypted) {
        printf("AES encrypted:[%d]%s\n", aes_encrypted_len, aes_encrypted);
    }
    size_t aes_decrypted_len = 0;
    char *aes_decrypted = kcrypto_aes_decode_alloc("13456",
                                                   aes_encrypted,
                                                   aes_encrypted_len,
                                                   &aes_decrypted_len);
    if (aes_decrypted) {
        printf("AES decrypted:[%d]%s\n", aes_decrypted_len, aes_decrypted);
    }

    if (aes_encrypted)
        free(aes_encrypted);
    if (aes_decrypted)
        free(aes_decrypted);

    return 0;
}