## 函数说明
- `kchar *kchar_new()` 创建 kchar * 对象，使用`kchar_free` 释放
    - return: 返回kchar *对象或NULL
- `void kchar_clean(kchar *x)` 仅释放x指向的数据，不包括x本身
    - x: 需要清理x指向的数据

- `void kchar_free(kchar *x)` 释放 x 资源，包括x本身

- `int kchar_copy(kchar *dst, char *data, unsigned int n)` 拷贝n个字节的data到dst内，如果dst内存不够会自动扩展后再拷贝
    - dst:      被拷贝的目标
    - data:     拷贝的源数据
    - n:        拷贝n个字节
    - return: 成功返回KCHAR_SUCC, 失败返回KCHAR_FAIL

- `int kchar_cat(kchar *dst, char *data, unsigned int n)` 拷贝n个字节的data追加到dst内，如果dst内存不够会自动扩展后再追加
    - dst:      被追加的目标
    - data:     拷贝的源数据
    - n:        拷贝n个字节
    - return: 成功返回KCHAR_SUCC, 失败返回KCHAR_FAIL

