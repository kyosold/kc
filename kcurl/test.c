/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 * Build:
 *  gcc -g -o test test.c kcurl.c -I/usr/include/curl/ -L/usr/lib64 -lcurl
 * Run:
 *  ./test
 * 
 */
#include <stdio.h>
#include <string.h>
#include "kcurl.h"

int main(void)
{
    int n;
    kcurl_list *headers = NULL;
    kcurl_response response;

    // Post
    printf("=============== POST ======================\n");
    // add custome header
    headers = kcurl_list_append(headers, "X-SPAM: YES");
    headers = kcurl_list_append(headers, "X-MID: F0769C030A03487A9D18D0BC4C8BF196");
    headers = kcurl_list_append(headers, "X-EMAIL: kyosold");

    char *url = "http://www.baidu.com";
    char *data = "user=kyosold&password=123qwe";
    n = kcurl_post(url, data, headers, 3, &response);
    if (n != 0) {
        printf("post fail\n");
        return 1;
    }
    printf("CODE: %d\n", response.code);
    printf("Header: \n%s\n", response.header.str);
    printf("BODY: \n%s\n", response.body.str);
    kcurl_response_clean(&response);

    // Get
    printf("=============== GET ======================\n");
    // add custome header
    headers = kcurl_list_append(NULL, "X-SPAM: YES");
    headers = kcurl_list_append(headers, "X-MID: F0769C030A03487A9D18D0BC4C8BF196");
    headers = kcurl_list_append(headers, "X-EMAIL: kyosold");
    
    char *url_get = "https://baike.baidu.com/item/fread/10942353";
    n = kcurl_get(url_get, headers, 3, &response);
    if (n != 0) {
        printf("get fail\n");
        return 1;
    }
    printf("CODE: %d\n", response.code);
    printf("Header: \n%s\n", response.header.str);
    printf("BODY: \n%s\n", response.body.str);
    kcurl_response_clean(&response);

    return 0;
}