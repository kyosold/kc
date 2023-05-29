#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "khashmap.h"

static uint32_t primes[] = 
{
    3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89,
    107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
    1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419,
    10103, 12143, 14591, 17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523,
    108631, 130363, 156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897,
    1162687, 1395263, 1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
};

/**
 * hashcode_index --- RShash
 * hashcode_create --- BKDRhash
*/
inline static uint32_t hashcode_index(char *key, int len, uint32_t size)
{
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    char *str = key;

    while (len-- > 0)
    {
        hash = hash * a + (*str++);
        a *= b;
    }

    return (uint32_t )(hash & 0x7FFFFFFF) % size;
}

inline static uint32_t hashcode_create(char *key, int len)
{
    uint32_t seed = 131;
    uint32_t hash = 0;
    char *str = key;

    while (len-- > 0)
        hash = hash * seed + (*str++);

    return (hash & 0x7FFFFFFF);
}

/* hashmap-link --- insert element into map */
void hashmap_link(khashmap *map, khashmap_node *elm)
{
    khashmap_node **etr = map->data + hashcode_index(elm->key,
                                                  elm->key_len,
                                                  map->size);
    if ((elm->next = *etr) != NULL)
        (*etr)->prev = elm;
    *etr = elm;
    map->used++;
}

/* hashmap_size --- allocate and initialize hash map */
static int hashmap_size(khashmap *map, unsigned int size)
{
    khashmap_node **n;

    map->data = n = (khashmap_node **)calloc(size, sizeof(khashmap_node *));
    if (!n) {
        return -1;
    }
    map->size = size;
    map->used = 0;

    while (size-- > 0)
        *n++ = 0;

    return 0;
}

/* hashmap_grow --- extend existing map */
static int hashmap_grow(khashmap *map)
{
    khashmap_node *ht;
    khashmap_node *next;
    unsigned old_size = map->size;
    khashmap_node **h = map->data;
    khashmap_node **old_nodes = h;

    if (hashmap_size(map, primes[++map->idx]) == -1)
        return -1;

    while (old_size-- > 0) {
        for (ht = *h++; ht; ht = next) {
            next = ht->next;
            hashmap_link(map, ht);
        }
    }

    if (old_nodes) {
        free(old_nodes);
        old_nodes = NULL;
    }

    return 0;
}


/**
 * @brief Create initial hash map
 * 
 * @param size 
 * @return khashmap* 
 */
khashmap *khashmap_create(int size)
{
    khashmap *map = (khashmap *)calloc(1, sizeof(khashmap));
    if (map == NULL) {
        return NULL;
    }

    uint8_t idx = 0;

    for (; idx < sizeof(primes) / sizeof(uint32_t); idx++) {
        if (size < primes[idx])
            break;
    }

    if (idx >= sizeof(primes) / sizeof(uint32_t))
        return NULL;

    map->idx = idx;

    return map;
}

/**
 * @brief enter (key, value) pair
 * 
 * @param map 
 * @param key 
 * @param key_len 
 * @param value 
 * @return khashmap_node* 
 */
khashmap_node *khashmap_insert(khashmap *map, char *key, uint32_t key_len, void *value)
{
    khashmap_node *ht = NULL;

    if ((map->used >= map->size) && hashmap_grow(map) == -1)
        return NULL;

    ht = (khashmap_node *)calloc(1, sizeof(khashmap_node) + key_len + 1);
    if (!ht)
        return NULL;
    
    // don't need free key, it just free ht
    ht->key = (char *)(ht + 1);
    memcpy(ht->key, key, key_len);
    ht->key_len = key_len;
    ht->value = strdup(value);
    ht->hash_code = hashcode_create(key, key_len);
    hashmap_link(map, ht);

    return ht;
}

/**
 * @brief lookup value
 * 
 * @param map 
 * @param key 
 * @param key_len 
 * @return void* 
 */
void *khashmap_find(khashmap *map, char *key, int key_len)
{
    khashmap_node *ht;
    uint32_t idx = hashcode_index(key, key_len, map->size);
    uint32_t hc = hashcode_create(key, key_len);

    if (map) {
        for (ht = map->data[idx]; ht; ht = ht->next) {
            if (key_len == ht->key_len && 
                hc == ht->hash_code && 
                (memcmp(key, ht->key, ht->key_len) == 0)
            ) {
                return ht->value;
            }
        }
    }
    return NULL;
}

/**
 * @brief lookup entry
 * 
 * @param map 
 * @param key 
 * @param key_len 
 * @return khashmap_node* 
 */
khashmap_node *khashmap_locate(khashmap *map, char *key, int key_len)
{
    khashmap_node *ht;
    uint32_t idx = hashcode_index(key, key_len, map->size);
    uint32_t hc = hashcode_create(key, key_len);

    if (map) {
        for (ht = map->data[idx]; ht; ht = ht->next) {
            if (key_len == ht->key_len 
                && hc == ht->hash_code 
                && (memcmp(key, ht->key, ht->key_len) == 0))
                return (ht);
        }
    }

    return NULL;
}

/**
 * @brief delete one entry
 * 
 * @param map 
 * @param key 
 * @param key_len 
 * @return int 
 */
int khashmap_delete(khashmap *map, char *key, int key_len)
{
    if (map != NULL) {
        khashmap_node **h = map->data + hashcode_index(key, key_len, map->size);
        khashmap_node *ht = *h;
        uint32_t hc = hashcode_create(key, key_len);

        for (; ht; ht = ht->next) {
            if (key_len == ht->key_len
                && hc == ht->hash_code
                && !memcmp(key, ht->key, ht->key_len)) {
                if (ht->next)
                    ht->next->prev = ht->prev;
                
                if (ht->prev)
                    ht->prev->next = ht->next;
                else
                    *h = ht->next;

                map->used--;

                // free ht item
                if (ht->value != NULL) {
                    free(ht->value);
                    ht->value = NULL;
                }

                if (ht != NULL) {
                    free(ht);
                    ht = NULL;
                }

                return 0;
            }
        }
    }

    return 1;
}

/**
 * @brief destroy hash map
 * 
 * @param map 
 */
void khashmap_free(khashmap *map)
{
    if (map != NULL) {
        unsigned i = map->size;
        khashmap_node *ht;
        khashmap_node *next;
        khashmap_node **h = map->data;

        while (i-- > 0) {
            for (ht = *h++; ht; ht = next) {
                next = ht->next;

                if (ht->value) {
                    free(ht->value);
                    ht->value = NULL;
                }
                if (ht) {
                    free(ht);
                    ht = NULL;
                }
            }
        }
        if (map->data) {
            free(map->data);
            map->data = NULL;
        }
        free(map);
        map = NULL;
    }
}

/**
 * @brief iterate over hash map
 * 
 * @param map 
 * @param action 
 * @param ptr 
 */
void khashmap_walk(khashmap *map, void (*action)(khashmap_node *, char *), char *ptr)
{
    if (map) {
        unsigned i = map->size;
        khashmap_node **h = map->data;
        khashmap_node *ht;

        while (i-- > 0)
            for (ht = *h++; ht; ht = ht->next)
                (*action)(ht, ptr);
    }
}

/**
 * @brief list all map members
 * 
 * @param map 
 * @return khashmap_node** 
 */
khashmap_node **khashmap_list(khashmap *map)
{
    khashmap_node **list;
    khashmap_node *member;
    int count = 0;
    int i = 0;

    if (map != NULL) {
        list = (khashmap_node **)calloc((map->used + 1), sizeof(*list));
        if (!list) {
            return NULL;
        }

        for (i = 0; i < map->size; i++)
            for (member = map->data[i]; member != 0; member = member->next)
                list[count++] = member;
    } else {
        list = (khashmap_node **)calloc(1, sizeof(*list));
        if (!list) {
            return NULL;
        }
    }
    list[count] = 0;

    return list;
}
