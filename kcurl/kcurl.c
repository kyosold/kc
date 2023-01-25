#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "kcurl.h"

static char kcurl_ca_pem[1024] = {0};

/**
 * @brief Add custom header, call kcurl_list_free_all to free
 * 
 * @param list      
 * @param value     The value to add header (key: val), including the terminating null byte ('\0')
 * example: 
 *  list = kcurl_list_append(list, "X-SJ: YES")
 */
kcurl_list *kcurl_list_append(kcurl_list *list, const char *header)
{
    return curl_slist_append(list, header);
}
/**
 * @brief Free custom header
 */
void kcurl_list_free_all(kcurl_list *list)
{
    curl_slist_free_all(list);
}

size_t _get_response_func(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    kcurl_data *resp = (kcurl_data *)user_p;
    if (resp->str == NULL) {
        resp->len = 0;
        resp->size = size * nmemb * 2 + 1;
        resp->str = (char *)malloc(resp->size);
        if (resp->str == NULL) {
            return 0;
        }
    }

    if ((resp->size - resp->len) < ((size * nmemb) + 1)) {
        unsigned int new_size = (resp->size * 2) + (size * nmemb) + 1;
        char *new_pstr = (char *)realloc(resp->str, new_size);
        if (new_pstr == NULL) {
            return 0;
        }
        resp->str = new_pstr;
        resp->size = new_size;
    }

    memcpy((resp->str + resp->len), buffer, (size * nmemb));
    resp->len += (size * nmemb);
    resp->str[resp->len] = '\0';

    return (size * nmemb);
}

/**
 * @brief 
 * 
 * @param pem 
 */
void kcurl_set_ca_pem(char *pem)
{
    snprintf(kcurl_ca_pem, sizeof(kcurl_ca_pem), "%s", pem);
}

/**
 * @brief 
 * 
 * @param url       The url address, include 'http/https'
 * @param data      The data to post
 * @param headers   The custom header, set NULL if do not
 * @param timeout   The timeout for connect and run
 * @param response  Store response information
 * @return unsigned int Return 0:succ, 1:fail
 */
unsigned int kcurl_post(char *url, char *data, kcurl_list *headers, unsigned int timeout, kcurl_response *response, char *err_str, size_t err_str_size)
{
    CURL *curl;
    CURLcode res;
    int ret = 0;

    /* In windows, this will init the winsock stuff */
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK)
        return 1;
    
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl == NULL)
        return 1;

    /* First set the URL that is about to receive our POST. */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    /* Now specify we want to POST data */
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Set custom header
    if (headers != NULL) {
        // initialize custom header list (stating that Expect: 100-continue is not wanted
        headers = curl_slist_append(headers, "Expect:");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Set post data
    if (data != NULL) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    }

    // Set SSL
    if (strncasecmp(url, "https", 5) == 0) {
        if (*kcurl_ca_pem != '\0')
            curl_easy_setopt(curl, CURLOPT_CAINFO, kcurl_ca_pem);

        /**
         * 0: 不检查证书
         * 1: 检查证书中是否有CN(common name)字段
         * 2: 检验当前的域名是否与CN匹配
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
        /**
         * 是否验证对方的证书
         * 0: 不检查证书的真实性
         * 1: 检查
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    }

    // 设置函数执行最长时间
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    // 设置连接服务器最长时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

    // 目前不考虑cookie情况
    // // 设置保存 cookie 到文件
    // if (save_cookie_fs != NULL) {
    //     curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1);
    //     curl_easy_setopt(curl, CURLOPT_COOKIEJAR, save_cookie_fs);
    // }

    // // 设置发送的cookie
    // if (send_cookie_fs != NULL) {
    //     curl_easy_setopt(curl, CURLOPT_COOKIEFILE, send_cookie_fs);
    // }

    // 设置处理响应header数据回调
    response->header.len = 0;
    response->header.size = 0;
    response->header.str = NULL;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _get_response_func);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (kcurl_data *)&response->header);

    // 设置接收响应body数据回调
    response->body.len = 0;
    response->body.size = 0;
    response->body.str = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _get_response_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (kcurl_data *)&response->body);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        snprintf(err_str, err_str_size, "[%d]:%s", res, curl_easy_strerror(res));
        ret = 1;
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->code);
        ret = 0;
    }

    if (headers != NULL) {
        kcurl_list_free_all(headers);
        headers = NULL;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return ret;
}

/**
 * @brief 
 * 
 * @param url       The url address, include 'http/https'
 * @param headers   The custom header, set NULL if do not
 * @param timeout   The timeout for connect and run
 * @param response  Store response information
 * @return unsigned int Return 0:succ, 1:fail
 */
unsigned int kcurl_get(char *url, kcurl_list *headers, unsigned int timeout, kcurl_response *response, char *err_str, size_t err_str_size)
{
    CURL *curl;
    CURLcode res;
    int ret = 0;

    /* In windows, this will init the winsock stuff */
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK)
        return 1;
    
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl == NULL)
        return 1;

    /* First set the URL that is about to receive our POST. */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // we tell libcurl to follow redirection
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Set custom header
    if (headers != NULL) {
        // initialize custom header list (stating that Expect: 100-continue is not wanted
        headers = curl_slist_append(headers, "Expect:");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Set SSL
    if (strncasecmp(url, "https", 5) == 0) {
        if (*kcurl_ca_pem != '\0')
            curl_easy_setopt(curl, CURLOPT_CAINFO, kcurl_ca_pem);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    }

    // 设置函数执行最长时间
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    // 设置连接服务器最长时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

    // 目前不考虑cookie情况
    // // 设置保存 cookie 到文件
    // if (save_cookie_fs != NULL) {
    //     curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1);
    //     curl_easy_setopt(curl, CURLOPT_COOKIEJAR, save_cookie_fs);
    // }

    // // 设置发送的cookie
    // if (send_cookie_fs != NULL) {
    //     curl_easy_setopt(curl, CURLOPT_COOKIEFILE, send_cookie_fs);
    // }

    // 设置处理响应header数据回调
    response->header.len = 0;
    response->header.size = 0;
    response->header.str = NULL;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _get_response_func);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (kcurl_data *)&response->header);

    // 设置接收响应body数据回调
    response->body.len = 0;
    response->body.size = 0;
    response->body.str = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _get_response_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (kcurl_data *)&response->body);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        snprintf(err_str, err_str_size, "[%d]:%s", res, curl_easy_strerror(res));
        ret = 1;
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->code);
        ret = 0;
    }

    if (headers != NULL) {
        kcurl_list_free_all(headers);
        headers = NULL;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return ret;
}


size_t _upload_data_func(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    // size_t n;
    // n = fread(buffer, size, nmemb, user_p);
    // return n;

    size_t n = 0;
    size_t len = 4096;
    char buf[4097];
    char *buf_encode = NULL;

    if (len > (size * nmemb))
        len = (size * nmemb);

    n = fread(buf, 1, len, user_p);

    // url encode
    CURL *_curl = curl_easy_init();
    if (_curl) {
        buf_encode = curl_easy_escape(_curl, buf, n);
        if (buf_encode) {
            memcpy(buffer, buf_encode, strlen(buf_encode));
            curl_free(buf_encode);
        }
        curl_easy_cleanup(_curl);
    }

    return n;
}
/**
 * @brief 
 * 
 * @param url       The url address, include 'http/https'
 * @param upload_file   
 * @param headers   The custom header, set NULL if do not
 * @param timeout   The timeout for connect and run
 * @param response  Store response information
 * @return unsigned int Return 0:succ, 1:fail
 */
unsigned int kcurl_put(char *url, char *upload_file, kcurl_list *headers, unsigned int timeout, kcurl_response *response, char *err_str, size_t err_str_size)
{
    if (upload_file == NULL)
        return 1;

    struct stat st;
    FILE *fp = NULL;


    if (stat(upload_file, &st) == -1)
    {
        return 1;
    }
    if ((st.st_mode & S_IFMT) != S_IFREG) {
        // file is not normal file
        return 1;
    }

    fp = fopen(upload_file, "rb");
    if (fp == NULL) {
        return 1;
    }

    CURL *curl;
    CURLcode res;
    int ret = 0;

    /* In windows, this will init the winsock stuff */
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        fclose(fp);
        return 1;
    }

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl == NULL) {
        fclose(fp);
        return 1;
    }

    /* First set the URL that is about to receive our POST. */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Set custom header
    if (headers != NULL) {
        // initialize custom header list (stating that Expect: 100-continue is not wanted
        headers = curl_slist_append(headers, "Expect:");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Set SSL
    if (strncasecmp(url, "https", 5) == 0) {
        if (*kcurl_ca_pem != '\0')
            curl_easy_setopt(curl, CURLOPT_CAINFO, kcurl_ca_pem);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    }

    // 设置函数执行最长时间
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    // 设置连接服务器最长时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

    // 目前不考虑cookie情况
    // // 设置保存 cookie 到文件
    // if (save_cookie_fs != NULL) {
    //     curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1);
    //     curl_easy_setopt(curl, CURLOPT_COOKIEJAR, save_cookie_fs);
    // }

    // // 设置发送的cookie
    // if (send_cookie_fs != NULL) {
    //     curl_easy_setopt(curl, CURLOPT_COOKIEFILE, send_cookie_fs);
    // }

    /* Set put */
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // libcurl 需要读取数据put到远程主机
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, _upload_data_func);
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)st.st_size);

    // 设置处理响应header数据回调
    response->header.len = 0;
    response->header.size = 0;
    response->header.str = NULL;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _get_response_func);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (kcurl_data *)&response->header);

    // 设置接收响应body数据回调
    response->body.len = 0;
    response->body.size = 0;
    response->body.str = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _get_response_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (kcurl_data *)&response->body);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        snprintf(err_str, err_str_size, "[%d]:%s", res, curl_easy_strerror(res));
        ret = 1;
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->code);
        ret = 0;
    }

    if (headers != NULL) {
        kcurl_list_free_all(headers);
        headers = NULL;
    }

    fclose(fp);

    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return ret;
}

void kcurl_response_clean(kcurl_response *response)
{
    if (response->header.str) {
        free(response->header.str);
        response->header.str = NULL;
        response->header.len = 0;
        response->header.size = 0;
    }
    if (response->body.str) {
        free(response->body.str);
        response->body.str = NULL;
        response->body.len = 0;
        response->body.size = 0;
    }
}
