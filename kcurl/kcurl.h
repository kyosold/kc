/**
 * @file kcurl.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __K_CURL_H__
#define __K_CURL_H__
#include <curl.h>

#define KCURL_TIMEOUT 3
#define KCURL_RETRY 3
#define KCURL_RETRY_INTERVAL 1

#define KCURL_MAX_HEADER 2048

typedef struct kcurl_data_st
{
    char *str;
    unsigned int len;
    unsigned int size;
} kcurl_data;

typedef struct kcurl_response_st
{
    unsigned long code;
    kcurl_data header;
    kcurl_data body;
} kcurl_response;

typedef struct curl_slist kcurl_list;

kcurl_list *kcurl_list_append(kcurl_list *list, const char *header);
void kcurl_list_free_all(kcurl_list *list);

void kcurl_set_ca_pem(char *pem);

unsigned int kcurl_post(char *url, char *data, kcurl_list *headers, unsigned int timeout, kcurl_response *response, char *err_str, size_t err_str_size);

unsigned int kcurl_get(char *url, kcurl_list *headers, unsigned int timeout, kcurl_response *response, char *err_str, size_t err_str_size);

unsigned int kcurl_put(char *url, char *upload_file, kcurl_list *headers, unsigned int timeout, kcurl_response *response, char *err_str, size_t err_str_size);

void kcurl_response_clean(kcurl_response *response);

#endif