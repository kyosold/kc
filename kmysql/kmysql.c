/**
 * @file kmysql.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kmysql.h"

/**
 * @brief Connect to mysql server
 * 
 * @param handle            The point to struct kmysql_conn from kmysql_init
 * @param err_str           Store error string
 * @param err_str_size      The size of err_str
 * @return int              Return 0 is succ, -1 is fail
 */
int kmysql_connect(kmysql_conn *handle, char *err_str, unsigned int err_str_size)
{
    MYSQL *ret_mysql;
    MYSQL *msp;
    int retval;

    if (handle == NULL)
        return -1;

    // init mysql lib
    msp = mysql_init(NULL);
    if (msp == NULL) {
        snprintf(err_str, err_str_size, "mysql_init fail");
        return -1;
    }

    retval = mysql_options(msp, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)(&handle->db_timeout));
    if (retval != 0) {
        snprintf(err_str, err_str_size, "mysql_init fail:%s", mysql_error(msp));
        mysql_close(msp);
        return -1;
    }
    retval = mysql_options(msp, MYSQL_SET_CHARSET_NAME, KMYSQL_CHARSET_NAME);
    if (retval != 0) {
        snprintf(err_str, err_str_size, "mysql_init fail:%s", mysql_error(msp));
        mysql_close(msp);
        return -1;
    }

    handle->msp = msp;
    ret_mysql = mysql_real_connect(
        handle->msp,
        handle->db_host,
        handle->db_user,
        handle->db_pass,
        handle->db_name,
        handle->db_port,
        NULL,
        // CLIENT_INTERACTIVE | CLIENT_MULTI_STATEMENTS);
        0);
    if (ret_mysql == NULL) {
        snprintf(err_str, err_str_size, "mysql_init fail:%s", mysql_error(handle->msp));
        mysql_close(handle->msp);
        handle->msp = NULL;
        return -1;
    }

    mysql_set_character_set(msp, "utf8");

    return 0;
}

/**
 * @brief Alloc a point to the struct kmysql_conn handle, call kmysql_free to free
 * 
 * @param db_host           The mysql server host
 * @param db_port           The mysql server port
 * @param db_user           The mysql server username
 * @param db_pass           The mysql server password
 * @param db_name           The mysql server database
 * @param timeout           The mysql server timeout
 * @param retry             The mysql server retry times
 * @param retry_interval    The mysql server retry interval, unit: second
 * @return kmysql_conn*     Return a point to the alloc struct kmysql_conn handle. On error, return NULL. Call kmysql_free() to free this handle
 */
kmysql_conn *kmysql_init(const char *db_host, unsigned int db_port, const char *db_user, const char *db_pass, const char *db_name, unsigned int timeout, unsigned int retry, unsigned int retry_interval)
{
    kmysql_conn *handle;
    int cur_num = 0;

    if (db_host == NULL || db_user == NULL || db_pass == NULL || db_name == NULL)
        return NULL;
    
    // create a mysql conn handle
    handle = (kmysql_conn *)calloc(1, sizeof(kmysql_conn));
    if (handle == NULL)
        return NULL;
    
    // save the mysql information
    snprintf(handle->db_host, sizeof(handle->db_host), "%s", db_host);
    snprintf(handle->db_user, sizeof(handle->db_user), "%s", db_user);
    snprintf(handle->db_pass, sizeof(handle->db_pass), "%s", db_pass);
    snprintf(handle->db_name, sizeof(handle->db_name), "%s", db_name);
    handle->db_port = db_port;
    handle->db_timeout = timeout;
    handle->db_retry = retry;
    handle->db_retry_interval = retry_interval;

    // // real connect to mysql server
    // while (1) {
    //     if (_mysql_connect(handle) == 0) {
    //         return handle;
    //     }
    //     if (++cur_num >= retry)
    //         break;
    // }

    // if (cur_num == 0)
    //     return handle;
    // else {
    //     free(handle);
    //     handle = NULL;
    // }
    return handle;
}

void kmysql_free(kmysql_conn *handle)
{
    if (handle == NULL)
        return;

    free(handle);
    handle = NULL;
}

void kmysql_disconnect(kmysql_conn *handle)
{
    if (handle == NULL)
        return;

    mysql_close(handle->msp);
    handle->msp = NULL;
    // free(handle);

    // https://stackoverflow.com/questions/8554585/mysql-c-api-memory-leak
    mysql_library_end();
}

/**
 * @brief 
 * 
 * @param handle 
 * @return const char*  Return error string
 */
const char *kmysql_error(kmysql_conn *handle)
{
    if (handle && handle->msp)
        return mysql_error(handle->msp);
    return "";
}

/**
 * @brief 转义SQL语句中特殊字符为使用的合法SQL字符串
 *
 * @param handle
 * @param from
 * @param from_len  from的长度，from内可以有\0
 * @param to        保存转义后的字符串并以\0结尾
 * @return unsigned long 返回to的长度(不计算\0)
 *
 * 使用方法:
 *  1. 单独转义
 *      char *from = "What's this";
 *      char to[strlen(from)*2+1];
 *      int n = kmysql_escape_string(conn, from, strlen(from), to);
 *  2. 连续转义
 *      char sql[1024], *end;
 *      end = strmov(sql, "insert into test_tbl values (");
 *      *end++ = '\'';
 *      end += kmysql_escape_string(conn, "what's this", to);
 *      *end++ = '\'';
 *      *end++ = ',';
 *      *end++ = '\'';
 *      end += kmysql_escape_string(conn, "binary data: \0\r\n", 16, to);
 *      *end++ = '\'';
 *      *end++ = ')';
 */
unsigned long kmysql_escape_string(kmysql_conn *handle, const char *from, unsigned long from_len, char *to)
{
    return mysql_real_escape_string(handle->msp, to, from, from_len);
}

/**
 * @brief For SQL: Insert/Update/Delete/Replace
 * 
 * @param handle 
 * @param sql_str 
 * @param sql_str_len 
 * @return int return the number of affected rows
 */
int kmysql_exec(kmysql_conn *handle, const char *sql_str, unsigned long sql_str_len)
{
    int retval = 0;
    int cur_times = 0;
    int affected_rows = 0;
    if (handle == NULL || handle->msp == NULL || sql_str == NULL)
        return affected_rows;

    for (;;) {
        retval = mysql_real_query(handle->msp, sql_str, sql_str_len);
        if (retval != 0) {
            if (cur_times++ < handle->db_retry) {
                if (handle->db_retry_interval > 0)
                    sleep(handle->db_retry_interval);
                continue;
            }
            break;
        }

        affected_rows = mysql_affected_rows(handle->msp);
        if (affected_rows <= 0) {
            if (cur_times++ < handle->db_retry) {
                if (handle->db_retry_interval > 0)
                    sleep(handle->db_retry_interval);
                continue;
            }
        }
        break;
    }

    return affected_rows;
}

void kmysql_result_clean(kmysql_result *result)
{
    int n = 0, i = 0;
    kmysql_row *p_row = result->row;
    kmysql_result *p = result->next;
    for (;;) {
        if (p_row == NULL)
            break;
        
        if (p_row->field) {
            for (i = 0; i < p_row->field_len; i++) {
                free(p_row->field[i]);
                p_row->field[i] = NULL;
            }
            free(p_row->field);
            p_row->field = NULL;
            p_row->field_len = 0;
        }
        free(p_row);

        p_row = p->row;
        p = p->next;
    }
    free(result);
    result = NULL;
}

kmysql_result *kmysql_query_alloc(kmysql_conn *handle, const char *sql_str, unsigned long sql_str_len)
{
    int retval = 0;
    int cur_times = 0;
    kmysql_result *result = NULL;
    
    if (handle == NULL || sql_str == NULL)
        return NULL;
    
    if (handle->msp == NULL)
        return NULL;

    for (;;) {
        retval = mysql_real_query(handle->msp, sql_str, sql_str_len);
        if (retval != 0) {
            if (cur_times++ < handle->db_retry) {
                if (handle->db_retry_interval > 0)
                    sleep(handle->db_retry_interval);
                continue;
            }
            break;
        } else {
            break;
        }
    }

    if (retval != 0) {
        return NULL;
    }

    MYSQL_RES *res = mysql_store_result(handle->msp);
    if (res == NULL) {
        return NULL;
    }

    int i = 0, j = 0, is_ok = 1;
    MYSQL_ROW row;
    unsigned int num_rows = mysql_num_rows(res);             // row number
    unsigned int num_fields = mysql_num_fields(res);      // field number

    result = (kmysql_result *)calloc(num_rows + 1, sizeof(kmysql_result));
    if (result == NULL) {
        mysql_free_result(res);
        return NULL;
    }
    while ((row = mysql_fetch_row(res))) {
        char **fields = (char **)calloc(num_fields + 1, sizeof(char *));
        if (fields == NULL) {
            kmysql_result_clean(result);
            break;
        }

        kmysql_row *krow = (kmysql_row *)malloc(sizeof(kmysql_row));
        if (krow == NULL) {
            kmysql_result_clean(result);
            break;
        }
        krow->field = fields;
        krow->field_len = num_fields;

        (result + i)->row = krow;
        (result + i)->next = result + (i + 1);

        for (j = 0; j < num_fields; j++) {
            fields[j] = (char *)malloc(strlen(row[j]) + 1);
            if (fields[j] == NULL) {
                is_ok = 0;
                break;
            }
            strcpy(fields[j], row[j]);
        }

        if (!is_ok) {
            kmysql_result_clean(result);
            break;
        }
        i++;
    }

    mysql_free_result(res);

    return result;
}

// /**
//  * @brief init sqllist, must call kmysql_sqllist_clean() to clean
//  * 
//  * @param number    The number of sql
//  * @return int  0:succ, 1:fail
//  */
// int kmysql_sqllist_init(unsigned int number)
// {
//     sqllist.number = number + 1;
//     sqllist.sql_list = (char **)calloc(sqllist.number, sizeof(char *));
//     if (sqllist.sql_list == NULL) {
//         return 1;
//     }
//     return 0;
// }

// void kmysql_sqllist_clean()
// {
//     if (sqllist.sql_list) {
//         free(sqllist.sql_list);
//         sqllist.sql_list = NULL;
//     }
//     sqllist.number = 0;
//     sqllist.len = 0;
// }

// void kmysql_sqllist_append(char *sql)
// {
//     if (sqllist.len >= sqllist.number)
//         return;

//     sqllist.sql_list[sqllist.len] = sql;
//     sqllist.len++;
// }

kmysql_list *kmysql_list_append(kmysql_list *list, const char *sql_str)
{
    char *dup_data = strdup(sql_str);
    if (!dup_data)
        return NULL;

    kmysql_list *last;
    kmysql_list *new_item;
    new_item = malloc(sizeof(kmysql_list));
    if (!new_item)
        return NULL;
    new_item->next = NULL;
    new_item->data = dup_data;

    if (!list)
        return new_item;

    // get last item
    last = list;
    while (last->next) {
        last = last->next;
    }
    last->next = new_item;

    return list;
}
void kmysql_list_free_all(kmysql_list *list)
{
    kmysql_list *next;
    kmysql_list *item;

    if (!list)
        return;

    item = list;
    do {
        next = item->next;
        free(item->data);
        free(item);
        item = next;
    } while (next);
}

/**
 * @brief 
 * 
 * @param handle 
 * @return int 0:succ, 1:fail
 */
int kmysql_transaction(kmysql_conn *handle, kmysql_list *sql_list)
{
    if (sql_list == NULL)
        return 1;
    if (handle == NULL || handle->msp == NULL)
        return 1;

    int retval = 0;
    int cur_times = 0;
    int affected_rows = 0;
    int i = 0;
    int is_ok = 1;
    kmysql_list *next;
    kmysql_list *item;

    // int ON = 1, OFF = 0;
    // mysql_autocommit(OFF);

    for (;;) {
        retval = mysql_real_query(handle->msp, "BEGIN", 5);
        if (retval != 0) {
            // 开启事务失败
            is_ok = 0;
        } else {
            is_ok = 1;
            item = sql_list;
            do {
                retval = mysql_real_query(handle->msp, item->data, strlen(item->data));
                if (retval != 0) {
                    is_ok = 0;
                    break;
                }
                next = item->next;
                item = next;
            } while (next);

            if (is_ok == 0) {
                mysql_real_query(handle->msp, "ROLLBACK", 8);
            } else {
                mysql_real_query(handle->msp, "COMMIT", 6);
            }
        }
        
        if (is_ok == 0) {
            if (cur_times++ < handle->db_retry) {
                if (handle->db_retry_interval > 0)
                    sleep(handle->db_retry_interval);
                continue;
                ;
            }
        }
        break;
    }
    return (!is_ok);
}
