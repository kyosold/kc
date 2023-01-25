## 1. 说明
对文件操作，由于文件太大，可能导致内存在申请连续空间时过慢，使用此库，优化为小块
内存块，使用链表串边起来，可优化内存使用速度。

## 2. kfile 图示
---

实例1 【kfile句柄】 -> 【数据块】 -> 【数据块】 -> 【数据块】 -> 【数据块】

实例2 【kfile句柄】 -> 【数据块】 -> 【数据块】 -> 【数据块】 -> 【数据块】

实例2 【kfile句柄】 -> 【数据块】 -> 【数据块】 -> 【数据块】 -> 【数据块】

## 3. 函数说明
---
kfile 的函数模仿unix系统io的接口实现，方便用户理解与使用。
- `KFILE *kopen(int data_block_size, USER_ALLOCER *af, USER_FREER *fe)`: 打开 kfile 句柄
    - data_block_size: 每个数据块的大小
    - af: 用户自定义的alloc函数
    - fe: 用户自定义的free函数
    - return: 返回指向KFILE的指针，使用kclose释放。失败返回NULL

- `void kclose(KFILE *handler)`: 关闭 kfile 句柄
    - handler: 需要关闭的句柄

- `void kseek(KFILE *handler)`: 读指针复位
    - handler: 需要复位的句柄

- `void kseek_pos(KFILE *handler, int pos)`: 读指针偏移
    - handler: 需要偏移的句柄
    - pos: 偏移的字节数

- `int kwrite(KFILE *handler, const char *data, int len)`: 向尾部存入数据
    - handler: 需要写入的句柄
    - data: 需要写入的数据
    - len: 写入数据的长度
    - return: 成功返回1，失败返回-1

- `int kwrite_head(KFILE *handler, const char *data, int len)`: 向头部存入数据
    - handler: 需要写入的句柄
    - data: 需要写入的数据
    - len: 写入数据的长度
    - return: 成功返回1，失败返回-1

- `int kread(KFILE *handler, char *data, int len)`: 读出数据
    - handler: 从该句柄读取
    - data: 保存读取数据的buffer
    - len: data的空间大小
    - return: 成功则返回读取的字节数，最后返回-1

- `char kgetc(KFILE *handler)`: 读出一个字符
    - handler: 从该句柄读取
    - return: 成功则返回读取的char，最后返回0

- `int kread_line(KFILE *handler, char *buf, int n)`: 读取一行
    - handler: 从该句柄读取
    - buf: 保存读取的一行的buffer
    - n: buf的空间大小
    - return: 成功则返回读取的字节数，最后返回0

- `int kwrite_file(KFILE *handler, int fd)`: 将数据全部写入文件
    - handler: 从该句柄读取
    - fd: 被写入数据的文件描述符
    - return: 成功则返回写入的字节数。失败返回-1

- `int ksize(KFILE *handler)`: 获取数据的大小

- `unsigned int kfile_new_header_size(KFILE *handler)`: 获取头部数据的大小

- `int kfile_copy(KFILE *dest, KFILE *src, unsigned int start, unsigned int end)`: 从src中拷贝start,end到dest内
    - dest: 目标KFILE指针
    - src: 源KFILE指针
    - start: 拷贝开始的索引
    - end: 拷贝结束的索引
    - return: 返回拷贝的字节数

