## 说明

khashmap 是为了给 key->value 键值对做快速查询使用的，对 key 进行 hash 可以快速查找到，value 在内部会被 strdup 一份出来，在使用`khashmap_free()`调用后，会被自动释放掉。

## 使用方式

1. 添加头文件

    ```c
    #include "khashmap.h"
    ```

2. 创建 map

    ```c
    khashmap *map = NULL;
    map = khashmap_create(1024);
    if (map == NULL) {
        return -1;
    }
    ```

3. 添加 key->value

    ```c
    char key[1024] = {0};
    char buf[1024] = {0};

    sprintf(key, "key_%x", 99);
    sprintf(buf, "val_%x", 288);
    khashmap_insert(map, key, strlen(key), buf);
    ```

4. 查找

    ```c
    sprintf(key, "key_%x", 99);
    printf("find key:%s val:%s\n", key, khashmap_find(map, key, strlen(key)));
    ```

5. 释放

    ```c
    khashmap_free(map);
    ```

## 编译

```bash
gcc -g -o test test khashmap
```
