## 说明

一些能用的函数功能。

## 函数说明

-   `void ktrim_str(char *str, int str_len, char *what, int mode, char *result, size_t result_size)` 对字符串做 trim 操作
    -   what: 指定被 trim 的字符，如果为 NULL，则使用' \t\n\r'
    -   mode: 对字符串哪部份做 trim 操作，1:left, 2:right, 3:left and right
