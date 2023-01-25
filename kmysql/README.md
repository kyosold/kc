## 一、说明
封装了简单操作mysql API方法

- 创建`kmysql_conn`handle指针，用`kmysql_free`释放
    ```
    kmysql_conn *kmysql_init(const char *db_host, unsigned int db_port, const char *db_user, const char *db_pass, const char *db_name, unsigned int timeout, unsigned int retry, unsigned int retry_interval)
    ```
    - db_host: 
    - db_port:
    - db_user:
    - db_pass:
    - db_name:
    - timeout: 超时时间
    - retry: 重试的次数
    - retry_interval: 重试之间的间隔时长，单位: 秒
    - return: 成功返回handle，失败返回NULL

- 连接mysql
    ```
    int kmysql_connect(kmysql_conn *handle, char *err_str, unsigned int err_str_size)
    ``` 
    - handle: 由`kmysql_init`创建的指针
    - err_str:  保存出错后的信息
    - err_str_size: err_str内存的大小
    - return: 成功返回0，失败返回-1

- 断开mysql
    ```
    void kmysql_disconnect(kmysql_conn *handle)
    ```
    - handle: 由`kmysql_init`创建的指针

- 释放handle
    ```
    void kmysql_free(kmysql_conn *handle)
    ```
    - handle: 由`kmysql_init`创建的指针

- 获取mysql错误信息
    ```
    const char *kmysql_error(kmysql_conn *handle)
    ```

- 字符转义
    ```
    unsigned long kmysql_escape_string(kmysql_conn *handle, const char *from, unsigned long from_len, char *to)
    ```
    - handle: 由`kmysql_init`创建的指针
    - from: 需要转义的内容（可能是二进制）
    - from_len: 由于from可能是二进制,所以必须指明长度
    - to: 保存转义后的结果, 空间大小为 (from_len * 2) + 1
    - return: 返回to的长度

- 执行影响条目的sql操作(Insert/Update/Delete/Replace)
    ```c
    int kmysql_exec(kmysql_conn *handle, const char *sql_str, unsigned long sql_str_len)
    ```
    - handle: 由`kmysql_init`创建的指针
    - sql_str: sql语句，转义后的语句
    - sql_str_len: sql_str的长度
    - return: 返回影响的行数，如果为0表示失败，调用`kmysql_error()`获取错误信息

- 执行查询的sql操作(select)
    ```c
    kmysql_result *kmysql_query_alloc(kmysql_conn *handle, const char *sql_str, unsigned long sql_str_len)
    ```
    - handle: 由`kmysql_init`创建的指针
    - sql_str: sql语句，转义后的语句
    - sql_str_len: sql_str的长度
    - return: 返回查询的结果，使用`kmysql_result_clean()`释放结果内存。如果为NULL表示失败，调用`kmysql_error()`获取错误信息

- 释放查询的sql的结果
    ```c
    void kmysql_result_clean(kmysql_result *result)
    ```
    - result: 由`kmysql_query_alloc`返回的非NULL的结果

- 追加sql到list
    ```c
    kmysql_list *klist_append(kmysql_list *list, const char *sql_str)
    ```
    - list: 被追加的list
    - sql_str: 追加的sql语句
    - return: 成功返回list，失败返回NULL

- 释放sqllist
    ```c
    void klist_free_all(kmysql_list *list)
    ```

- 执行sqllist事务sql操作
    ```c
    int kmysql_transaction(kmysql_conn *handle, kmysql_list *sql_list)
    ```
    - handle: 由`kmysql_init`创建的指针
    - sql_list: sql list语句
    - return: 成功返回0，失败返回1，调用`kmysql_error()`获取错误信息
    > **此函数执行sql list内的语句，所以依赖上面的kmysql_list_函数簇**

## 二、使用方式

### 1. 执行Insert/Update/Delete/Replace
```c
#include "kmysql.h"

char err_str[1024];
kmysql_conn *mysql = kmysql_init(DB_HOST, atoi(DB_PORT),
                                     DB_USER, DB_PASS, DB_NAME,
                                     3, 5, 1);
if (mysql == NULL) {
    fprintf(stderr, "alloc struct kmysql_conn handle fail\n");
    return 1;
}
int n = kmysql_connect(mysql, err_str, sizeof(err_str));
if (n != 0) {
    fprintf(stderr, "connect to mysql(%s:%s) user(%s) pass(%s) db(%s) fail:%s\n", 
        DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_NAME, err_str);
    return 1;
}

char sql[4096];
snprintf(sql, sizeof(sql), "insert into gpk_user (email, password) values ('apple', '666666')");
int affected = kmysql_exec(mysql, sql, strlen(sql));
printf("affected row:%d\n", affected);

kmysql_disconnect(mysql);
kmysql_free(mysql);
```

### 2. 执行查询Select
```c
#include "kmysql.h"

char err_str[1024];
kmysql_conn *mysql = kmysql_init(DB_HOST, atoi(DB_PORT),
                                     DB_USER, DB_PASS, DB_NAME,
                                     3, 5, 1);
if (mysql == NULL) {
    fprintf(stderr, "alloc struct kmysql_conn handle fail\n");
    return 1;
}
int n = kmysql_connect(mysql, err_str, sizeof(err_str));
if (n != 0) {
    fprintf(stderr, "connect to mysql(%s:%s) user(%s) pass(%s) db(%s) fail:%s\n", 
        DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_NAME, err_str);
    return 1;
}

char sql[4096];
snprintf(sql, sizeof(sql), "select * from gpk_pubkey");
kmysql_result *result = kmysql_query_alloc(mysql, sql, strlen(sql));
if (result == NULL) {
    fprintf(stderr, "query sql(%s) fail:%s\n", sql, kmysql_error(mysql));
    kmysql_disconnect(mysql);
    return 1;
}

// show result
int i = 0;
kmysql_result *p = result;
while (p->row != NULL) {
    if (p->row->field) {
        printf("-------------------------------\n");
        for (i = 0; i < p->row->field_len; i++) {
            printf("| %s ", p->row->field[i]);
        }
        printf("\n-------------------------------");
    }
    p = p->next;
}

kmysql_result_clean(result);

kmysql_disconnect(mysql);
kmysql_free(mysql);
```

### 3. 执行多条事务
```c
#include "kmysql.h"

char err_str[1024];
kmysql_conn *mysql = kmysql_init(DB_HOST, atoi(DB_PORT),
                                     DB_USER, DB_PASS, DB_NAME,
                                     3, 5, 1);
if (mysql == NULL) {
    fprintf(stderr, "alloc struct kmysql_conn handle fail\n");
    return 1;
}
int n = kmysql_connect(mysql, err_str, sizeof(err_str));
if (n != 0) {
    fprintf(stderr, "connect to mysql(%s:%s) user(%s) pass(%s) db(%s) fail:%s\n", 
        DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_NAME, err_str);
    return 1;
}

char s1[1024], s2[1024], s3[1024];
snprintf(s1, sizeof(s1), "insert into gpk_user (email, password) values ('trans1', '111111')");
snprintf(s2, sizeof(s2), "insert into gpk_user (email, password) values ('trans2', '222222')");
snprintf(s3, sizeof(s3), "insert into gpk_user (email, password) values ('trans3', '333333')");

/**
 * 注意，如果不对 kmysql_list *list = NULL 初始化为NULL，则在第1次append时，
 * 必须写为: list = kmysql_list_append(NULL, s1);
 */
kmysql_list *list = NULL;
list = kmysql_list_append(list, s1);
list = kmysql_list_append(list, s2);
list = kmysql_list_append(list, s3);

int ret = kmysql_transaction(mysql, list);
if (ret == 0) {
    printf("transaction succ\n");
} else {
    printf("transaction fail:%s\n", kmysql_error(mysql));
}
kmysql_list_free_all(list);

kmysql_disconnect(mysql);
kmysql_free(mysql);
```

### 4. 转义SQL特殊字符
- 单独使用
    ```c
    char *from = "What's this";
    char to[strlen(from)*2+1];
    int n = kmysql_escape_string(mysql, from, strlen(from), to);
    ```
- 连续使用
    ```c
    char sql[1024], *end;
    end = strmov(sql, "insert into test_tbl values (");
    *end++ = '\'';
    *end++ = ',';
    *end++ = '\'';
    end += kmysql_escape_string(conn, "binary data: \0\r\n", 16, to);
    *end++ = '\'';
    *end++ = ')';
    ```

## 三、编译
依赖`mariadb-devel`
```bash
gcc -g -o test test.c kmysql.c -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient
```
