#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/conf.h>
#include <openssl/aes.h>
#include "kcrypto.h"

static char err_str[1024];

char *kcrypto_errstr()
{
    return err_str;
}

//////////////////////////////
// Hash
//////////////////////////////
/**
 * @brief Calculate hash value
 * 
 * @param method            The type of hash
 * @param data 
 * @param data_len 
 * @param out               The result of hash values
 * @param out_size          The length of result
 * @return unsigned int     Return 0 is fail, other is succ
 */
unsigned int kcrypto_hash(KAlgorithm method, char *data, size_t data_len, char *out, size_t out_size)
{
    if (out_size < EVP_MAX_MD_SIZE) {
        snprintf(err_str, sizeof(err_str), "the memory size of out must be greater than %ld", EVP_MAX_MD_SIZE);
        return 0;
    }

    int i = 0;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    EVP_MD_CTX *ctx;
    const EVP_MD *md;

    if (method == KCRYPTO_MD4) {
        md = EVP_md4();
    } else if (method == KCRYPTO_MD5) {
        md = EVP_md5();
    } else if (method == KCRYPTO_SHA1) {
        md = EVP_sha1();
    } else if (method == KCRYPTO_SHA224) {
        md = EVP_sha224();
    } else if (method == KCRYPTO_SHA256) {
        md = EVP_sha256();
    } else if (method == KCRYPTO_SHA384) {
        md = EVP_sha384();
    } else if (method == KCRYPTO_SHA512) {
        md = EVP_sha512();
    } else {
        snprintf(err_str, sizeof(err_str), "not support method:%d", method);
        return 0;
    }

    // EVP_MD_CTX *ctx = EVP_MD_CTX_new();  // openssl-devel >= 1.1.0
    ctx = EVP_MD_CTX_create();  // < 1.1.0
    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestUpdate(ctx, data, data_len);
    EVP_DigestFinal_ex(ctx, md_value, &md_len);
    // EVP_MD_CTX_free(ctx);    // >= 1.1.0
    EVP_MD_CTX_destroy(ctx);    // < 1.1.0

    for (i = 0; i < md_len; ++i) {
        sprintf(&out[i * 2], "%02X", md_value[i]);
    }

    return (md_len * 2);
}

//////////////////////////////
// Base64
//////////////////////////////
/**
 * @brief Encodes data with MIME base64
 * 
 * @param data      The data to encode
 * @param data_len  The length of data
 * @param out_len   The length of encoded data
 * @return char*    The encoded data, as a string, NULL is fail
 */
char *kcrypto_base64_encode_alloc(char *data, size_t data_len, size_t *out_len)
{
    BIO *bio, *b64;
    BUF_MEM *buffer_ptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines - write everyting in one line

    BIO_write(bio, data, data_len);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buffer_ptr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    int len = buffer_ptr->length + 1;
    char *result = (char *)malloc(len);
    if (result == NULL) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        return NULL;
    }
    memset(result, 0, len);
    memcpy(result, buffer_ptr->data, buffer_ptr->length);
    *out_len = buffer_ptr->length;

    return result;
}

/**
 * @brief   Decodes data encoded with MIME base64
 * 
 * @param data      The decoded data
 * @param data_len  The length of data
 * @param out_len   The length of result
 * @return char*    Return the decoded data or NULL is fail.
 * @note The returned data may be binary, out_len is result_len.
 */
char *kcrypto_base64_decode_alloc(char *data, size_t data_len, size_t *out_len)
{
    BIO *bio, *b64;
    int len;
    char *result = NULL;

    len = data_len + 1;
    result = (char *)malloc(len);
    if (result == NULL) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        return NULL;
    }
    memset(result, 0, len);

    bio = BIO_new_mem_buf(data, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Do not use newlines to flush buffer

    *out_len = BIO_read(bio, result, data_len);

    BIO_free_all(bio);

    return result;
}

//////////////////////////////
// RSA
//////////////////////////////
#define PADDING RSA_PKCS1_PADDING
#define KEYSIZE 32
#define IVSIZE 32
#define BLOCKSIZE 256
#define SALTSIZE 8

#define BEGIN_RSA_PUBLIC_KEY "BEGIN RSA PUBLIC KEY"
#define BEGIN_PUBLIC_KEY "BEGIN PUBLIC KEY"
#define OPENSS_ERR ERR_error_string(ERR_get_error(), NULL)

typedef enum
{
    PUBLIC_ENC,
    PRIVATE_ENC,
    PUBLIC_DEC,
    PRIVATE_DEC
} KCRYPTO_RSA_TYPE;
char *process_bigdata_with_rsa(KCRYPTO_RSA_TYPE type, RSA *rsa, char *data, size_t data_len, size_t *out_len)
{
    char str[1024];
    // 根据 data_len 的长度，对数据块分块加密
    int rsa_len = RSA_size(rsa);
    int block_len = rsa_len;
    if (type == PUBLIC_ENC || type == PRIVATE_ENC) {
        // 因为填充方式为RSA_PKCS1_PADDING, 所以要在rsaLen基础上减去11
        block_len -= 11;
    }

    int slice = data_len / block_len;
    if ((data_len % block_len) != 0)
        slice++;

    // 分配接收数据的总长度
    int n_out_size = slice * rsa_len;
    int p_out_len = 0;
    unsigned char *p_out = (unsigned char *)malloc(n_out_size + 1);
    if (!p_out) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        return NULL;
    }
    memset(p_out, 0, n_out_size + 1);

    // 分配每次存放 buffer 的空间
    unsigned char *buf = (unsigned char *)malloc(rsa_len + 1);
    if (!buf) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        free(p_out);
        return NULL;
    }

    char *p_sub = NULL;
    int ret = 0, pos = 0, i = 0;
    while (pos < data_len) {
        memset(buf, 0, rsa_len + 1);
        if ((data_len - pos) < block_len)
            block_len = (data_len - pos);
        p_sub = data + pos;

        if (type == PUBLIC_ENC) {
            snprintf(str, sizeof(str), "RSA_public_encrypt");
            ret = RSA_public_encrypt(block_len,
                                     (unsigned char *)p_sub,
                                     (unsigned char *)buf,
                                     rsa,
                                     PADDING);
        } else if (type == PUBLIC_DEC) {
            snprintf(str, sizeof(str), "RSA_public_decrypt");
            ret = RSA_public_decrypt(block_len,
                                     (unsigned char *)p_sub,
                                     (unsigned char *)buf,
                                     rsa,
                                     PADDING);
        } else if (type == PRIVATE_ENC) {
            snprintf(str, sizeof(str), "RSA_private_encrypt");
            ret = RSA_private_encrypt(block_len,
                                      (unsigned char *)p_sub,
                                      (unsigned char *)buf,
                                      rsa,
                                      PADDING);
        } else if (type == PRIVATE_DEC) {
            snprintf(str, sizeof(str), "RSA_private_decrypt");
            ret = RSA_private_decrypt(block_len,
                                      (unsigned char *)p_sub,
                                      (unsigned char *)buf,
                                      rsa,
                                      PADDING);
        }

        if (ret < 0) {
            snprintf(err_str, sizeof(err_str), "%s fail:%s", str,OPENSS_ERR);
            free(buf);
            free(p_out);
            return NULL;
        }

        memcpy(p_out + p_out_len, buf, ret);
        p_out_len += ret;
        pos += block_len;
        i++;
    }
    free(buf);

    *out_len = p_out_len;
    return p_out;
}

void _initalize()
{
    ERR_load_CRYPTO_strings();
    OpenSSL_add_all_algorithms();
}
void _finalize()
{
    EVP_cleanup();
    ERR_free_strings();
}

void kcrypto_keypairs_free(struct kcrypto_keyparis *keyparis)
{
    if (keyparis == NULL)
        return;

    if (keyparis->private_key_alloc != NULL) {
        free(keyparis->private_key_alloc);
    }
    if (keyparis->public_key_alloc != NULL) {
        free(keyparis->public_key_alloc);
    }
    free(keyparis);
}
/**
 * @brief   Generate public and private key pairs
 * 
 * @param bits 
 * @return struct kcrypto_keyparis* 
 */
struct kcrypto_keyparis *kcrypto_keypairs_alloc(KCRYPTO_RSA_Bits bits)
{
    int ret = 0;
    RSA *rsa = NULL;
    BIGNUM *bne = NULL;
    BIO *bio_pub = NULL, *bio_pri = NULL;
    unsigned long e = RSA_3;
    size_t pub_len = 0, pri_len = 0;
    struct kcrypto_keyparis *result = NULL;

    result = (struct kcrypto_keyparis *)malloc(sizeof(struct kcrypto_keyparis));
    if (result == NULL) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        return NULL;
    }

    _initalize();

    // 1. generate rsa key
    bne = BN_new();
    if (!bne) {
        snprintf(err_str, sizeof(err_str), "BN_new fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }
    ret = BN_set_word(bne, e);
    if (ret != 1) {
        snprintf(err_str, sizeof(err_str), "BN_set_word fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }
    rsa = RSA_new();
    if (!rsa) {
        snprintf(err_str, sizeof(err_str), "RSA_new fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }

    ret = RSA_generate_key_ex(rsa, bits, bne, NULL);
    if (ret != 1) {
        snprintf(err_str, sizeof(err_str), "RSA_generate_key_ex fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }

    // 2. save private key
    bio_pri = BIO_new(BIO_s_mem());
    if (!bio_pri) {
        snprintf(err_str, sizeof(err_str), "BIO_new fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }
    ret = PEM_write_bio_RSAPrivateKey(bio_pri, rsa, NULL, NULL, 0, NULL, NULL);
    if (ret != 1) {
        snprintf(err_str, sizeof(err_str), "PEM_write_bio_RSAPrivateKey fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }
    pri_len = BIO_pending(bio_pri);
    result->private_key_alloc = (char *)malloc(pri_len + 1);
    if (result->private_key_alloc == NULL) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        goto KCRYPTO_EXIT;
    }
    memset(result->private_key_alloc, 0, pri_len + 1);
    BIO_read(bio_pri, result->private_key_alloc, pri_len);

    // 3. save public key
    bio_pub = BIO_new(BIO_s_mem());
    if (!bio_pub) {
        snprintf(err_str, sizeof(err_str), "BIO_new fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }
    // 生成第1种格式公钥：PEM_write_bio_RSAPublicKey(bio_pub, rsa)
    ret = PEM_write_bio_RSA_PUBKEY(bio_pub, rsa);   // 这里生成第2种格式
    if (ret != 1) {
        snprintf(err_str, sizeof(err_str), "PEM_write_bio_RSA_PUBKEY fail:%s", OPENSS_ERR);
        goto KCRYPTO_EXIT;
    }
    pub_len = BIO_pending(bio_pub);
    result->public_key_alloc = (char *)malloc(pub_len + 1);
    if (result->public_key_alloc == NULL) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        goto KCRYPTO_EXIT;
    }
    memset(result->public_key_alloc, 0, pub_len + 1);
    BIO_read(bio_pub, result->public_key_alloc, pub_len);

    ret = 1;

KCRYPTO_EXIT:
    if (bio_pub)
        BIO_free_all(bio_pub);
    if (bio_pri)
        BIO_free_all(bio_pri);
    if (rsa)
        RSA_free(rsa);
    if (bne)
        BN_free(bne);
    if (ret != 1) {
        kcrypto_keypairs_free(result);
        result = NULL;
    }

    _finalize();

    return result;
}


RSA *new_rsa_key(char *text_key, int is_public_key)
{
    char str[1024] = {0};
    RSA *rsa = NULL;
    BIO *in = BIO_new_mem_buf((unsigned char *)text_key, strlen(text_key));
    if (in == NULL) {
        snprintf(err_str, sizeof(err_str), "BIO_new_mem_buf fail:%s", OPENSS_ERR);
        return NULL;
    }
    // 如果打开 BIO_FLAGS_BASE64_NO_NL，则传入时要带"\n"（即"Hello\n")，
    // 否则，还原后会丢失两个字符
    BIO_set_flags(in, BIO_FLAGS_BASE64_NO_NL);

    rsa = RSA_new();
    if (is_public_key) {
        if (strstr(text_key, BEGIN_RSA_PUBLIC_KEY)) {
            snprintf(str, sizeof(str), "PEM_read_bio_RSAPublicKey");
            rsa = PEM_read_bio_RSAPublicKey(in, &rsa, NULL, NULL);
        } else {
            snprintf(str, sizeof(str), "PEM_read_bio_RSA_PUBKEY");
            rsa = PEM_read_bio_RSA_PUBKEY(in, &rsa, NULL, NULL);
        }
    } else {
        snprintf(str, sizeof(str), "PEM_read_bio_RSAPrivateKey");
        rsa = PEM_read_bio_RSAPrivateKey(in, &rsa, NULL, NULL);
    }
    if (rsa == NULL) {
        snprintf(err_str, sizeof(err_str), "%s fail:%s", str, OPENSS_ERR);
        BIO_free_all(in);
        return NULL;
    }
    BIO_free(in);
    return rsa;
}
void free_rsa_key(RSA *rsa)
{
    RSA_free(rsa);
}

/**
 * @brief   Encrypts data with public key
 * 
 * @param text_key  The public key
 * @param data 
 * @param data_len 
 * @param out_len 
 * @return char*    Return encrypted data (base64 encoded) or NULL is fail.
 */
char *kcrypto_public_encode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len)
{
    char *encrypted = NULL;

    // 1. 获取 RSA*
    RSA *rsa = new_rsa_key(text_key, 1);
    if (rsa == NULL)
        return encrypted;

    // 2. base64 encode
    size_t data_b64_len = 0;
    char *data_b64 = kcrypto_base64_encode_alloc(data, data_len, &data_b64_len);
    if (data_b64 == NULL) {
        free_rsa_key(rsa);
        return encrypted;
    }

    // 3. encrypt
    size_t encrypted_len = 0;
    encrypted = process_bigdata_with_rsa(PUBLIC_ENC, rsa, data_b64, strlen(data_b64), &encrypted_len);
    if (encrypted == NULL) {
        free(data_b64);
        free_rsa_key(rsa);
        return NULL;
    }
    free(data_b64);

    // 4 return base64
    char *encrypted_b64 = kcrypto_base64_encode_alloc(encrypted, encrypted_len, &data_b64_len);
    free(encrypted);

    *out_len = data_b64_len;
    return encrypted_b64;
}

/**
 * @brief   Decrypts data with private key
 * 
 * @param text_key  The private key
 * @param data 
 * @param data_len 
 * @param out_len   The length of result
 * @return char*    Return decrypts data or NULL is fail. 
 * @note Returned data may be binary, length is out_len.
 */
char *kcrypto_private_decode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len)
{
    char *decrypted = NULL;

    // 1. 获取 RSA*
    RSA *rsa = new_rsa_key(text_key, 0);
    if (rsa == NULL)
        return decrypted;

    // 2. base64 decode
    size_t data_b64_len = 0;
    char *data_b64_de = kcrypto_base64_decode_alloc(data, data_len, &data_b64_len);
    if (data_b64_de == NULL) {
        free_rsa_key(rsa);
        return NULL;
    }

    // 3. decrypted
    size_t decrypted_len = 0;
    decrypted = process_bigdata_with_rsa(PRIVATE_DEC, rsa, data_b64_de, data_b64_len, &decrypted_len);
    if (decrypted == NULL) {
        free(data_b64_de);
        free_rsa_key(rsa);
        return NULL;
    }
    free(data_b64_de);

    // 4. return base64 decode
    size_t res_len = 0;
    char *result = kcrypto_base64_decode_alloc(decrypted, decrypted_len, &res_len);
    free(decrypted);

    *out_len = res_len;

    return result;
}

/**
 * @brief   Encrypts data with private key
 * 
 * @param text_key  The private key
 * @param data 
 * @param data_len 
 * @param out_len 
 * @return char*    Return encrypts data or NULL is fail.
 */
char *kcrypto_private_encode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len)
{
    char *encrypted = NULL;

    // 1. 获取 RSA*
    RSA *rsa = new_rsa_key(text_key, 0);
    if (rsa == NULL)
        return encrypted;
    
    // 2. base64 encode
    size_t data_b64_len = 0;
    char *data_b64 = kcrypto_base64_encode_alloc(data, data_len, &data_b64_len);
    if (data_b64 == NULL) {
        free_rsa_key(rsa);
        return encrypted;
    }

    // 3. encrypt
    size_t encrypted_len = 0;
    encrypted = process_bigdata_with_rsa(PRIVATE_ENC, rsa, data_b64, strlen(data_b64), &encrypted_len);
    if (encrypted == NULL) {
        free(data_b64);
        free_rsa_key(rsa);
        return NULL;
    }
    free(data_b64);

    // 4 return base64
    char *encrypted_b64 = kcrypto_base64_encode_alloc(encrypted, encrypted_len, &data_b64_len);
    free(encrypted);

    *out_len = data_b64_len;
    return encrypted_b64;
}

/**
 * @brief   Decrypts data iwth public key
 * 
 * @param text_key  The public key
 * @param data 
 * @param data_len 
 * @param out_len   The length of result
 * @return char*    Return decrypts data or NULL is fail.
 * @note Returned data may be binary, length is out_len.
 */
char *kcrypto_public_decode_alloc(char *text_key, char *data, size_t data_len, size_t *out_len)
{
    char *decrypted = NULL;

    // 1. 获取 RSA*
    RSA *rsa = new_rsa_key(text_key, 1);
    if (rsa == NULL)
        return decrypted;

    // 2. base64 decode
    size_t data_b64_len = 0;
    char *data_b64_de = kcrypto_base64_decode_alloc(data, data_len, &data_b64_len);
    if (data_b64_de == NULL) {
        free_rsa_key(rsa);
        return NULL;
    }

    // 3. decrypted
    size_t decrypted_len = 0;
    decrypted = process_bigdata_with_rsa(PUBLIC_DEC, rsa, data_b64_de, data_b64_len, &decrypted_len);
    if (decrypted == NULL) {
        free(data_b64_de);
        free_rsa_key(rsa);
        return NULL;
    }
    free(data_b64_de);

    // 4. return base64 decode
    size_t res_len = 0;
    char *result = kcrypto_base64_decode_alloc(decrypted, decrypted_len, &res_len);
    free(decrypted);

    *out_len = res_len;
    return result;
}

//////////////////////////////
// AES
//////////////////////////////
#define KEYSIZE 32
#define IVSIZE  32
#define BLOCKSIZE   256
#define SALTSIZE    8

char *kcrypto_aes_encode_alloc(char *passphrase, char *data, size_t data_len, size_t *out_len)
{
    unsigned char arr[SALTSIZE];
    RAND_bytes(arr, SALTSIZE);
    int rounds = 1;
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];

    const unsigned char *salt = (const unsigned char *)&arr;
    const unsigned char *password = (const unsigned char *)passphrase;

    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(),
                           salt, password, strlen(passphrase),
                           rounds, key, iv);
    if (i != KEYSIZE) {
        snprintf(err_str, sizeof(err_str), "EVP_BytesToKey fail:%s",OPENSS_ERR);
        return NULL;
    }

    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(en);

    if (!EVP_EncryptInit_ex(en, EVP_aes_256_cbc(), NULL, key, iv)) {
        snprintf(err_str, sizeof(err_str), "EVP_EncryptInit_ex fail:%s",OPENSS_ERR);
        EVP_CIPHER_CTX_free(en);
        return NULL;
    }

    int c_len = data_len + AES_BLOCK_SIZE, f_len = 0;
    unsigned char *pout = (unsigned char *)malloc(SALTSIZE + c_len);
    if (pout == NULL) {
        snprintf(err_str, sizeof(err_str), "malloc fail:%s", strerror(errno));
        EVP_CIPHER_CTX_free(en);
        return NULL;
    }
    memcpy(pout, salt, SALTSIZE);
    unsigned char *ciphertext = pout + SALTSIZE;

    if (!EVP_EncryptInit_ex(en, NULL, NULL, NULL, NULL)) {
        snprintf(err_str, sizeof(err_str), "EVP_EncryptInit_ex fail:%s",OPENSS_ERR);
        free(ciphertext);
        EVP_CIPHER_CTX_free(en);
        return NULL;
    }

    // May have to repeat this for large files
    if (!EVP_EncryptUpdate(en, ciphertext, &c_len, (unsigned char *)data, data_len)) {
        snprintf(err_str, sizeof(err_str), "EVP_EncryptUpdate fail:%s",OPENSS_ERR);
        free(ciphertext);
        EVP_CIPHER_CTX_free(en);
        return NULL;
    }

    if (!EVP_EncryptFinal(en, ciphertext+c_len, &f_len)) {
        snprintf(err_str, sizeof(err_str), "EVP_EncryptFinal fail:%s",OPENSS_ERR);
        free(ciphertext);
        EVP_CIPHER_CTX_free(en);
        return NULL;
    }

    int len = c_len + f_len;
    EVP_CIPHER_CTX_cipher(en);
    EVP_CIPHER_CTX_free(en);

    // 结果做base64 encode
    char *decrypted_b64 = NULL;
    size_t decrypted_b64_len = 0;
    decrypted_b64 = kcrypto_base64_encode_alloc(pout, len+SALTSIZE, &decrypted_b64_len);

    free(pout);

    *out_len = decrypted_b64_len;
    return decrypted_b64;
}

char *kcrypto_aes_decode_alloc(char *passphrase, char *data, size_t data_len, size_t *out_len)
{
    char msalt[SALTSIZE];
    char *in = NULL;
    size_t in_len = 0;
    int rounds = 1;
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];

    // base64 decode
    size_t data_b64ed_len = 0;
    char *data_b64ed = kcrypto_base64_decode_alloc(data, data_len, &data_b64ed_len);
    if (data_b64ed == NULL) {
        return NULL;
    }

    memcpy(msalt, data_b64ed, SALTSIZE);
    in = data_b64ed + SALTSIZE;
    in_len = data_b64ed_len - SALTSIZE;

    const unsigned char *salt = (const unsigned char *)msalt;
    const unsigned char *password = (const unsigned char *)passphrase;

    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(),
                           salt, password, strlen(passphrase),
                           rounds, key, iv);
    if (i != KEYSIZE)
    {
        snprintf(err_str, sizeof(err_str), "EVP_BytesToKey fail:%s",OPENSS_ERR);
        free(data_b64ed);
        return NULL;
    }

    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(de);

    if (!EVP_DecryptInit_ex(de, EVP_aes_256_cbc(), NULL, key, iv)) {
        snprintf(err_str, sizeof(err_str), "EVP_DecryptInit_ex fail:%s",OPENSS_ERR);
        EVP_CIPHER_CTX_free(de);
        free(data_b64ed);
        return NULL;
    }

    int p_len = in_len, f_len = 0;
    unsigned char *plaintext = (unsigned char *)malloc(p_len + AES_BLOCK_SIZE);
    if (plaintext == NULL) {
        snprintf(err_str, sizeof(err_str), "EVP_DecryptInit_ex fail:%s",OPENSS_ERR);
        EVP_CIPHER_CTX_free(de);
        free(data_b64ed);
        return NULL;
    }

    // May have to do this multiple times for large data
    if (!EVP_DecryptUpdate(de, plaintext, &p_len, (unsigned char *)in, in_len)) {
        snprintf(err_str, sizeof(err_str), "EVP_DecryptInit_ex fail:%s",OPENSS_ERR);
        EVP_CIPHER_CTX_free(de);
        free(plaintext);
        free(data_b64ed);
        return NULL;
    }

    if (!EVP_DecryptFinal_ex(de, plaintext+p_len, &f_len)) {
        snprintf(err_str, sizeof(err_str), "EVP_DecryptFinal_ex fail:%s",OPENSS_ERR);
        EVP_CIPHER_CTX_free(de);
        free(plaintext);
        free(data_b64ed);
        return NULL;
    }
    free(data_b64ed);

    int len = p_len + f_len;
    EVP_CIPHER_CTX_cleanup(de);
    EVP_CIPHER_CTX_free(de);

    *out_len = len;
    return plaintext;
}
