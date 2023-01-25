/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 * Build:
 *  gcc -g -o test test.c kbase64.c
 * Run:
 *  ./test
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kbase64.h"
#include "kurl.h"

int main(void)
{
    int n = 0;

    // base64 encode
    printf("\n------ base64 encode ------\n");
    char *be_str = "hello, world!";
    unsigned int be_res_size = KBASE64_LENGTH(strlen(be_str));
    char be_res[be_res_size];
    n = kbase64_encode(be_str, strlen(be_str), be_res, be_res_size);
    printf("%s\n", be_str);
    printf("[%d]%s\n", n, be_res);

    // base64 decode
    printf("\n------ base64 decode ------\n");
    char *bd_str = "5L2g5ZGo5pyr6YO95bmy5Zib5LqG77yf";
    unsigned int bd_res_size = strlen(bd_str);
    char bd_res[bd_res_size];
    n = kbase64_decode(bd_str, strlen(bd_str), bd_res, bd_res_size);
    printf("%s\n", bd_str);
    printf("[%d]%s\n", n, bd_res);

    // url encode
    printf("\n------ url encode ------\n");
    char *ue_str = "greeting kyosold@qq.com 31% MB";
    int ue_res_size = KURL_LENGTH(strlen(ue_str));
    char ue_res[ue_res_size];
    n = kurl_encode(ue_str, strlen(ue_str), ue_res, ue_res_size);
    printf("%s\n", ue_str);
    printf("[%d]%s\n", n, ue_res);

    // url decode
    printf("\n------ url encode ------\n");
    char *ud_str = "greeting+kyosold%40qq.com+31%25+MB";
    int ud_res_size = strlen(ud_str);
    char ud_res[ud_res_size];
    n = kurl_decode(ud_str, strlen(ud_str), ud_res, ud_res_size);
    printf("%s\n", ud_str);
    printf("[%d]%s\n", n, ud_res);

    // url raw encode
    printf("\n------ url raw encode ------\n");
    char *uer_str = "greeting kyosold@qq.com 31% MB";
    int uer_res_size = KURL_LENGTH(strlen(uer_str));
    char uer_res[uer_res_size];
    n = kurl_raw_encode(uer_str, strlen(uer_str), uer_res, uer_res_size);
    printf("%s\n", uer_str);
    printf("[%d]%s\n", n, uer_res);

    // url raw decode
    printf("\n------ url raw decode ------\n");
    char *udr_str = "greeting%20kyosold%40qq.com+31%25+MB";
    int udr_res_size = strlen(udr_str);
    char udr_res[udr_res_size];
    n = kurl_decode(udr_str, strlen(udr_str), udr_res, udr_res_size);
    printf("%s\n", udr_str);
    printf("[%d]%s\n", n, udr_res);


    ///////// alloc ///////////
    // alloc base64 encode
    printf("\n------ alloc base64 encode ------\n");
    char *be_str_a = "hello, world!";
    unsigned int be_res_a_len = 0;
    char *be_res_a = kbase64_encode_alloc(be_str_a, strlen(be_str_a), &be_res_a_len);
    if (be_res_a == NULL) {
        printf("base64 encode fail\n");
        return 1;
    }
    printf("%s\n", be_str_a);
    printf("[%d]%s\n", be_res_a_len, be_res_a);
    free(be_res_a);

    // alloc base64 decode
    printf("\n------ alloc base64 decode ------\n");
    char *bd_str_a = "5L2g5ZGo5pyr6YO95bmy5Zib5LqG77yf";
    unsigned int bd_res_a_len = 0;
    char *bd_res_a = kbase64_decode_alloc(bd_str_a, strlen(bd_str_a), &bd_res_a_len);
    if (bd_res_a == NULL) {
        printf("base64 decode fail\n");
        return 1;
    }
    printf("%s\n", bd_str_a);
    printf("[%d]%s\n", bd_res_a_len, bd_res_a);
    free(bd_res_a);

    // alloc url encode
    printf("\n------ alloc url encode ------\n");
    char *ue_str_a = "greeting kyosold@qq.com 31% MB";
    int ue_res_a_len = 0;
    char *u_res_a = kurl_encode_alloc(ue_str_a, strlen(ue_str_a), &ue_res_a_len);
    if (u_res_a == NULL) {
        printf("url encode fail\n");
        return 1;
    }
    printf("%s\n", ue_str_a);
    printf("[%d]%s\n", ue_res_a_len, u_res_a);
    free(u_res_a);

    // alloc url decode
    printf("\n------ alloc url raw encode ------\n");
    char *uer_str_a = "greeting%20kyosold%40qq.com%2031%25%20MB";
    int uer_res_a_len = 0;
    char *uer_res_a = kurl_raw_encode_alloc(uer_str_a, strlen(uer_str_a), &uer_res_a_len);
    if (uer_res_a == NULL) {
        printf("url raw encode fail\n");
        return 1;
    }
    printf("%s\n", uer_str_a);
    printf("[%d]%s\n", uer_res_a_len, uer_res_a);
    free(uer_res_a);

    return 0;
}