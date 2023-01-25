#include <stdio.h>
#include <string.h>

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

