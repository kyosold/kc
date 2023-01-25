/**
 * Build:
 *  gcc -g -o test test.c kmysql.c -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient
 * Run:
 *  ./test
 * 
 */

#include <stdio.h>
#include <string.h>
#include "kmysql.h"

#define DB_HOST  "127.0.0.1"
#define DB_PORT  "3306"
#define DB_USER  "root"
#define DB_PASS  "123qwe"
#define DB_NAME  "gpkeys"

int main(void)
{
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
    // Insert
    printf("\n====== Insert ======\n");
    snprintf(sql, sizeof(sql), "insert into gpk_user (email, password) values ('apple', '666666')");
    int affected = kmysql_exec(mysql, sql, strlen(sql));
    printf("affected row:%d\n", affected);

    // Select
    printf("\n====== Select ======\n");
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

    // Transaction
    printf("\n====== Transcation ======\n");
    char s1[1024], s2[1024], s3[1024];
    snprintf(s1, sizeof(s1), "insert into gpk_user (email, password) values ('trans1', '111111')");
    snprintf(s2, sizeof(s2), "insert into gpk_user (email, password) values ('trans2', '222222')");
    snprintf(s3, sizeof(s3), "insert into gpk_user (email, password) values ('trans3', '333333')");

    kmysql_list *list = NULL;               // 注意如果这里不给NULL
    list = kmysql_list_append(list, s1);    // 如果上面不写NULL，则这里要写: list = kmysql_list_append(NULL, s1);
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

    return 0;
}