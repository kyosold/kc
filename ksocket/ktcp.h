#ifndef __KTCP_H__
#define __KTCP_H__

#include <sys/types.h>
#include <sys/socket.h>

int ktcp_connect(char *host, char *port, int timeout, char *err, size_t err_size);
int ktcp_read(int sockfd, char *buf, int len, int timeout);
int ktcp_write(int sockfd, char *buf, int len, int timeout);

#endif