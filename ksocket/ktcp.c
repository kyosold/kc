#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "ktcp.h"

int io_timeout(int type, int fd, char *buf, int len, int timeout);

int connect_timeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int timeout)
{
    int flags, n, err;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tv;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    err = 0;
    if ((n = connect(sockfd, addr, addrlen)) < 0)
        if (errno != EINPROGRESS)
            return -1;

    /* Do whatever we want while the connect is taking place. */
    if (n == 0)
        goto done;  // connect completed immediately

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    n = select(sockfd + 1, &rset, &wset, NULL, timeout ? &tv : NULL);
    if (n == 0) {   // timeout
        close(sockfd);
        errno = ETIMEDOUT;
        return -2;
    }
    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(err);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
            return -1;  // Solaris pending error
    } else {
        // select error: sockfd not set
        close(sockfd);
        return -1;
    }

    done:
    fcntl(sockfd, F_SETFL, flags);  // restore file status flags

    if (err) {
        close(sockfd);  // just in case
        errno = err;
        return -1;
    }

    return 0;
    }

/**
 * @brief 
 * 
 * @param host 
 * @param port 
 * @param timeout 
 * @param err 
 * @param err_size 
 * @return int          The socket file descriptor of connected on success, -1 on error.
 */
int ktcp_connect(char *host, char *port, int timeout, char *err, size_t err_size)
{
    int sockfd = -1, n;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        snprintf(err, err_size, "getaddrinfo: %s", gai_strerror(rv));
        return -1;
    }

    // 以循环找出全部的结果，并先连线到能成功连线的
    int i = 0;
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        n = connect_timeout(sockfd, p->ai_addr, p->ai_addrlen, timeout);
        if (n == -1) {      // error
            close(sockfd);
            i++;
            continue;
        }
        if (n == -2) {      // timeout
            close(sockfd);
            i++;
            continue;
        }

        break;
    }

    if (p == NULL) {
        snprintf(err, err_size, "all %d of the connections failed", i);
        return -1;
    }

    freeaddrinfo(servinfo);     // 释放 servinfo 内存空间

    return sockfd;
}

/**
 * @brief Read len bytes from the sockfd to buf
 *
 * @param sockfd    socket file descriptor
 * @param buf       buffer to read data from
 * @param len       length of buffer
 * @param timeout   seconds
 * @return int      number of bytes read on success;
 *                  0 when remote closed;
 *                  -1 on error and check errno
 *                  -2 on timeout
 */
int ktcp_read(int sockfd, char *buf, int len, int timeout)
{
    int flags, n;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    for (;;) {
        n = io_timeout(0, sockfd, buf, len, timeout);
        if (n == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                continue;
        }

        break;
    }

    return n;
}

/**
 * @brief Writs len bytes from buf to the sockfd
 * 
 * @param sockfd    socket file descriptor
 * @param buf       buffer to write data to
 * @param len       length of buffer
 * @param timeout   seconds
 * @return int      number of bytes written on success
 *                  0 when remote closed
 *                  -1 on error and check errno
 *                  -2 on timeout
 */
int ktcp_write(int sockfd, char *buf, int len, int timeout)
{
    int flags, n;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    for (;;) {
        n = io_timeout(1, sockfd, buf, len, timeout);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
        }
        break;
    }

    return n;
}

/**
 * @brief 添加超时设置的IO
 *
 * @param type      0:read, 1:write
 * @param fd        file descriptor
 * @param buf       buffer to read data from or write data to
 * @param len       length of buffer
 * @param timeout   seconds
 * @return int      -2 on timeout; 
 *                  -1 on error and check errno; 
 *                  0 on reaching EOF;
 *                  number of bytes read on success
 */
int io_timeout(int type, int fd, char *buf, int len, int timeout) 
{
    fd_set fds;
    int n;
    struct timeval tv;

    // 设定fd
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    // 设定timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // 一直等到timeout或收到消息
    if (type == 0)
        n = select(fd+1, &fds, NULL, NULL, &tv);    // read
    else
        n = select(fd+1, NULL, &fds, NULL, &tv);    // write

    if (n == 0) return -2;  // timeout;
    if (n == -1) return -1; // error

    if (type == 0)
        return read(fd, buf, len);
    else
        return write(fd, buf, len);
}

