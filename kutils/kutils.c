#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Strip whitespace (or other characters) from the beginning and end of a string
 * 
 * @param str 
 * @param str_len 
 * @param what      Indicates which chars are to be trimmed. NULL is default (' \n\r\t')
 * @param mode      1:trim left, 2:trim right, 3:trim left and right
 */
void ktrim_str(char *str, int str_len, char *what, int mode, char *result, size_t result_size)
{
    int len = str_len;
    int i = 0, j = 0;
    int matched = 0;
    int trimmed = 0;
    char mask[256];

    if (what) {
        snprintf(mask, sizeof(mask), "%s", what);
    } else {
        snprintf(mask, sizeof(mask), " \n\r\t");
    }

    if (mode & 1) {
        for (i = 0; i < len; i++) {
            matched = 0;
            j = 0;
            while (j < strlen(mask)) {
                if (mask[j++] == str[i]) {
                    matched = 1;
                    break;
                }
            }

            if (matched) {
                trimmed++;
            } else {
                break;
            }
        }
        len -= trimmed;
        str += trimmed;
    }
    if (mode & 2) {
        for (i = len - 1; i >= 0; i--) {
            matched = 0;
            j = 0;
            while (j < strlen(mask)) {
                if (mask[j++] == str[i]) {
                    matched = 1;
                    break;
                }
            }

            if (matched) {
                len--;
            } else {
                break;
            }
        }
    }

    memcpy(result, str, len);
    result[len] = '\0';
}

/**
 * @brief Execute an external program and return the output
 * 
 * @param cmd       The command that will be executed.
 * @return char*    Returns the command output on success, and NULL on error and check errno
 */
char *ksystem_alloc(char *cmd)
{
    FILE *fp;
    int len = 0;
    int size = 4096;
    int new_size = 0;

    char *buf = (char *)malloc(size);
    if (buf == NULL) {
        return NULL;
    }

    if ((fp = popen(cmd, "r")) == NULL) {
        free(buf);
        return NULL;
    }

    while (fgets(buf+len, (size-len-1), fp) != NULL) {
        if ((size - len) < strlen(buf)) {
            new_size = (size * 2);
            char *tmp = (char *)realloc(buf, new_size);
            if (tmp == NULL) {
                pclose(fp);
                free(buf);
                return NULL;
            }
            buf = tmp;
            size = new_size;
        }
        len = strlen(buf);
    }

    pclose(fp);

    if (len == 0) {
        free(buf);
        return NULL;
    }

    return buf;
}

/**
 * @brief Execute an external program and return
 * 
 * @param cmd 
 * @param response 
 * @param response_size 
 * @return int 
 */
int ksystem(char *cmd, char *response, size_t response_size)
{
    FILE *fp;
    int len = 0;

    char *resp = response;
    memset(resp, 0, response_size);

    if ((fp = popen(cmd, "r")) == NULL) {
        return 1;
    }
    while (fgets(resp + len, response_size - len, fp) != NULL) {
        len = strlen(resp);
    }

    pclose(fp);

    if (len == 0)
        return 1;

    return 0;
}

