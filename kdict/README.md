## 1. 函数说明
---

- `kdict *kdict_new(int size)`: 创建字典对象
    - size: 初始化字典的大小，不知道的话给0
    > 该函数分配一个给定大小的新字典对象并返回它。如果不知道字典的条目，则给出size=0
    > 使用 kdict_free 释放资源

- `void kdict_free(kdict *d)`: 释放字典对象
    - d: 要释放的字典对象
    > 释放一个字典对象及其关联的所有内存资源

- `int kdict_set(kdict *d, char *key, char *val)`: 在字典中设置一个键值对
    - d: 要修改的字典对象
    - key: 用于修改或添加的键
    - val: 修改或添加的值
    - 返回0为成功，其它为错误
    > 1. 如果在字典中找到给定的键，则值被替换。如果未找到，则添加。
    > 2. 为val设置NULL值是可以的，但为字典或键提供NUL值，则是错误的。
    > 3. 注意如果kdict_set一个NULL值，则kdict_get返回一个NULL值。如果key已经存在，设置NULL值，相当于释放了之前的值


- `char *kdict_get(kdict *d, char *key, char *def)`: 从字典中获取值
    - d: 要搜索的字典对象
    - key: 在字典中查找的键
    - def: 如果没找到键，则返回该值
    - 返回指向内部分配字符串的指针
    > 该函数在给定的字典中定位一个键，并返回指向它值的指针，如果找不到，则返回传递的'def'指针。

    **注意：返回的字符指针，不应该释放或修改它**

- `void kdict_unset(kdict *d, char *key)`: 删除字典中的键值对
    - d: 要修改的字典对象
    - key: 要删除的键
    > 该函数删除字典中的一个键。如果未找到键，则什么都不做

## 2. 使用方法
---
1. 初始化字典对象:
```c
kdict *d = kdict_new(0);
```
2. 设置键值对:
```c
char key[128];
char value[128];
snprintf(key, sizeof(key), "ConentType");
snprintf(value, sizeof(value), "Plain");
kdict_set(d, key, value);
```
3. 获取值:
```c
char *val = kdict_get(d, key, NULL);
if (val == NULL) {
    /* key not found */
    return;
}
printf("%s -> %s\n", key, val);
```
4. 删除键:
```c
kdict_unset(d, key);
```
5. 删除字典对象:
```c
kdict_free(d);
```
