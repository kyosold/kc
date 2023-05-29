#include <stdio.h>
#include <string.h>
#include "khashmap.h"

void hash_test_func(khashmap *map)
{
    char key[1024] = {0};
    char buf[1024] = {0};
    int i = 0;
    int j = 0;

    for (; i < 9; i++) {
        j = 0xffffff - i;
        sprintf(key, "key_%x", j);
        sprintf(buf, "val_%x", j);

        khashmap_insert(map, key, strlen(key), buf);
    }

    for (i = 0; i < 9; i++) {
        j = 0xffffff - i;
        sprintf(buf, "key_%x", j);
        printf("find key:%s val:%s\n", buf, khashmap_find(map, buf, strlen(buf)));
    }
}

int main(int argc, char **argv)
{
    khashmap *map = NULL;
    map = khashmap_create(102400);
    if (map == NULL) {
        return -1;
    }

    hash_test_func(map);

    khashmap_free(map);

    return 0;
}
