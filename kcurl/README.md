## 一、函数说明

- `kcurl_list *kcurl_list_append(kcurl_list *list, const char *header)` 添加自定义头，使用后，执行get/post/put后会自动释放，或手动`kcurl_list_free_all`释放
    - list: 添加的list，如果是第一次调用，必须给NULL
    - value: 自定义头内容，如 (X-SPAM: YES)

- `void kcurl_list_free_all(kcurl_list *list)` 释放自定义头list的资源

- `unsigned int kcurl_get(char *url, kcurl_list *headers, unsigned int timeout, kcurl_response *response)` Get提交，会自动释放自定义头资源
    - url: 提交的url地址
    - headers: 添加的自定义头，如果没有则给NULL
    - timeout: 连接和执行的超时时间，单位：秒
    - response: 存储server返回的信息，必须手动调用`kcurl_response_clean`释放资源

- `unsigned int kcurl_post(char *url, char *data, kcurl_list *headers, unsigned int timeout, kcurl_response *response)` Post提交，会自动释放自定义头资源
    - url: 提交的url地址
    - data: post提交的数据
    - headers: 添加的自定义头，如果没有则给NULL
    - timeout: 连接和执行的超时时间，单位：秒
    - response: 存储server返回的信息，必须手动调用`kcurl_response_clean`释放资源

- `unsigned int kcurl_put(char *url, char *upload_file, kcurl_list *headers, unsigned int timeout, kcurl_response *response)` Put提交，会自动释放自定义头资源
    - url: 提交的url地址
    - upload_file: 需要put的文件路径
    - headers: 添加的自定义头，如果没有则给NULL
    - timeout: 连接和执行的超时时间，单位：秒
    - response: 存储server返回的信息，必须手动调用`kcurl_response_clean`释放资源

- `void kcurl_response_clean(kcurl_response *response)` 释放server返回信息的资源
    - response: 存储server返回的信息

## 二、编译
- 依赖: `libcurl-devel`
```bash
gcc -g -o test test.c kcurl.c -I/usr/include/curl -L/usr/lib64 -lcurl
```

