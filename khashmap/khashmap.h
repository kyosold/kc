/**
 * @file khashmap.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-05-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __K_HASHMAP_H__
#define __K_HASHMAP_H__

#include <stdio.h>
#include <stdint.h>

typedef struct khashmap_node_st khashmap_node;

struct khashmap_node_st
{
    char *key; // lookup key
    uint32_t key_len;
    uint32_t hash_code;  // hash code
    char *value;         // associated value
    khashmap_node *next; // colliding entry
    khashmap_node *prev; // colliding entry
};

typedef struct khashmap_st
{
    uint32_t size;        // length of entries array
    uint32_t used;        // number of entries in table
    uint8_t idx;          // primes id
    khashmap_node **data; // entries array, auto-resized
} khashmap;

khashmap *khashmap_create(int size);
khashmap_node *khashmap_insert(khashmap *map, char *key, uint32_t key_len, void *value);
void *khashmap_find(khashmap *map, char *key, int key_len);
khashmap_node *khashmap_locate(khashmap *map, char *key, int key_len);
int khashmap_delete(khashmap *map, char *key, int key_len);
void khashmap_free(khashmap *map);
void khashmap_walk(khashmap *map, void (*action)(khashmap_node *, char *), char *ptr);
khashmap_node **khashmap_list(khashmap *map);


#endif