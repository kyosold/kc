/**
 * @file kmysql.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __KMYSQL_H__
#define __KMYSQL_H__

#include <stdint.h>
#include <stdlib.h>
#include <mysql.h>

#define KMYSQL_CONN_NAME_LEN 256
#define KMYSQL_CONN_TIMEOUT 3
#define KMYSQL_CHARSET_NAME "utf8"
#define KMYSQL_RETRY 3

#define KMYSQL_LEN(len) (len * 2) + 1

typedef struct _kmysql_handle
{
    MYSQL *msp;
    char db_host[KMYSQL_CONN_NAME_LEN];
    char db_user[KMYSQL_CONN_NAME_LEN];
    char db_pass[KMYSQL_CONN_NAME_LEN];
    char db_name[KMYSQL_CONN_NAME_LEN];
    unsigned int db_port;
    unsigned int db_timeout;
    unsigned int db_retry;          // 重试次数
    unsigned int db_retry_interval; // 重试间隔时长，单位秒
} kmysql_conn;

typedef struct _kmysql_row_st
{
    char **field;
    unsigned int field_len;
} kmysql_row;
typedef struct _kmysql_result_st kmysql_result;
struct _kmysql_result_st
{
    kmysql_row *row;
    kmysql_result *next; // 下一个
};


kmysql_conn *kmysql_init(const char *db_host, unsigned int db_port, const char *db_user,
                         const char *db_pass, const char *db_name, unsigned int timeout,
                         unsigned int retry, unsigned int retry_interval);
int kmysql_connect(kmysql_conn *handle, char *err_str, unsigned int err_str_size);
void kmysql_disconnect(kmysql_conn *handle);
void kmysql_free(kmysql_conn *handle);

const char *kmysql_error(kmysql_conn *handle);

unsigned long kmysql_escape_string(kmysql_conn *handle, const char *from, unsigned long from_len, char *to);

int kmysql_exec(kmysql_conn *handle, const char *sql_str, unsigned long sql_str_len);

kmysql_result *kmysql_query_alloc(kmysql_conn *handle, const char *sql_str, unsigned long sql_str_len);

void kmysql_result_clean(kmysql_result *result);


// linked-list structure
typedef struct kmysql_list_st kmysql_list;
struct kmysql_list_st
{
    char *data;
    kmysql_list *next;
};
kmysql_list *kmysql_list_append(kmysql_list *list, const char *sql_str);
void kmysql_list_free_all(kmysql_list *list);
int kmysql_transaction(kmysql_conn *handle, kmysql_list *sql_list);

#endif