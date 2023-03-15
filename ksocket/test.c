/**
 * @file test.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * Build:
 *  gcc -g -o test test.c ktcp.c
 * 
 * Run:
 *  ./test smtp.sina.net 25 2
 * 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "ktcp.h"

int main(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: %s <ip> <port> <timeout>\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    char *port = argv[2];
    int timeout = atoi(argv[3]);

    char buf[1024];

    int n = 0;
    char err[1024];
    int sockfd = ktcp_connect(host, port, 3, err, sizeof(err));
    if (sockfd == -1) {
        printf("error:%s\n", err);
        return 1;
    }

    n = ktcp_read(sockfd, buf, sizeof(buf), timeout);
    printf("read:[%d]%s", n, buf);

    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf), stdin);
    printf("fgets:[%d]%s", n, buf);

    n = ktcp_write(sockfd, buf, sizeof(buf), timeout);
    printf("write:[%d]%s", n, buf);

    memset(buf, 0, sizeof(buf));
    n = ktcp_read(sockfd, buf, sizeof(buf), timeout);
    printf("read:[%d]%s", n, buf);

    close(sockfd);

    return 0;
}