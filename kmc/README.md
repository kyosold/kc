## 一、函数说明
- Set MC
    ```c
    unsigned int kmc_set(char *mc_host, int mc_port, unsigned int timeout,
                     char *key, unsigned int key_len, char *value, unsigned int value_len, time_t expire,
                     char *err, unsigned int err_size)
    ```
    - mc_host: MC Server 地址
    - mc_port: MC Server 端口
    - timeout: 连接MC超时时间，单位:秒
    - key: 设置的key
    - key_len: key的长度，因为key有可能是二进制
    - value: 对应的值
    - value_len: 值的长度，因为value有可能是二进制
    - err: 保存错误信息，如果不需要可以设为NULL
    - err_size: err的空间大小，不需要设为0
    - return: 成功返回0，失败返回1

- Get MC
    ```c
    char *kmc_get(char *mc_host, int mc_port, unsigned int timeout,
              char *key, unsigned int key_len, int *value_len,
              char *err, unsigned int err_size)
    ```
    - mc_host: MC Server 地址
    - mc_port: MC Server 端口
    - timeout: 连接MC超时时间，单位:秒
    - key: 获取的key
    - key_len: key的长度，因为key有可能是二进制
    - value_len: value的长度或者-1:失败，0:未找到
    - err: 保存错误信息，如果不需要可以设为NULL
    - err_size: err的空间大小，不需要设为0
    - return: 返回key对应的值，出错返回NULL，检查value_len表示不同的错误

- Delete MC
    ```c
    unsigned int kmc_del(char *mc_host, int mc_port, unsigned int timeout,
                     char *key, unsigned int key_len,
                     char *err, unsigned int err_size)
    ```
    - mc_host: MC Server 地址
    - mc_port: MC Server 端口
    - timeout: 连接MC超时时间，单位:秒
    - key: 删除的key
    - key_len: key的长度，因为key有可能是二进制
    - err: 保存错误信息，如果不需要可以设为NULL
    - err_size: err的空间大小，不需要设为0
    - return: 成功返回0，1为未找到，2为失败

## 二、编译
依赖 `libmemcached-devel`
```bash
gcc -g -o test test.c kmc.c -I/usr/include/libmemcached -L/usr/lib64/ -lmemcached
```
