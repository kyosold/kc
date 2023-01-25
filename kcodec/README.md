## 1. Base64 使用方法

---
- **encode:**
    ```c
    #include "kbase64.h"

    char *str = "hello, world!";
    int res_size = KBASE64_LENGTH(strlen(str));
    char res[res_size];
    int n = kbase64_encode(str, strlen(str), res, res_size);
    printf("[%d]%s\n", n, res);
    ```
- **decode:**
    ```c
    #include "kbase64.h"

    char *str = "5L2g5ZGo5pyr6YO95bmy5Zib5LqG77yf";
    unsigned int res_size = strlen(str);
    char res[res_size];
    int n = kbase64_decode(str, strlen(str), res, res_size);
    printf("[%d]%s\n", n, res);
    ```

## 2. URL 使用方法
---
- **url encode:**
    ```c
    #include "kurl.h"

    char *str = "greeting kyosold@qq.com 31% MB";
    int res_size = KURL_LENGTH(strlen(str));
    char res[res_size];
    n = kurl_encode(str, strlen(str), res, res_size);
    printf("[%d]%s\n", n, res);
    ```
- **url decode:**
    ```c
    #include "kurl.h"

    char *str = "greeting+kyosold%40qq.com+31%25+MB";
    int res_size = strlen(str);
    char res[res_size];
    n = kurl_decode(str, strlen(str), res, res_size);
    printf("[%d]%s\n", n, res);
    ```

- **url raw encode:**
    ```c
    #include "kurl.h"

    char *str = "greeting kyosold@qq.com 31% MB";
    int res_size = KURL_LENGTH(strlen(str));
    char res[res_size];
    n = kurl_raw_encode(str, strlen(str), res, res_size);
    printf("[%d]%s\n", n, res);
    ```
- **url raw decode:**
    ```c
    #include "kurl.h"

    char *str = "greeting%20kyosold%40qq.com+31%25+MB";
    int res_size = strlen(str);
    char res[res_size];
    n = kurl_decode(str, strlen(str), res, res_size);
    printf("[%d]%s\n", n, res);
    ```

- url encode 与 url raw encode 的区别：前者是把空格转成+号，后者是把空格转成%20
