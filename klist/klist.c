/**
 * @file klist.c
 * @author songjian (kyosold@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <string.h>
#include "klist.h"

/* return last node in linked list */
static klist *klist_last(klist *list)
{
    klist *item;
    
    // if caller passed us a NULL, return now
    if (!list)
        return NULL;
    
    // loop through to find the last item
    item = list;
    while (item->next) {
        item = item->next;
    }
    return item;
}

/*
* klist_append_nodup() appends a string to end linked list.
* Rather than copying the string in dynamic storage, it takes
* its ownership. The string should have been malloc()ated. 
* klist_append_nodup always returns the address of the first 
* recode, so that you can use this function as an initialization
* function as well as an append function.
* If an error occurs, NULL is returned and the string argument
* is NOT released.
*/
klist *klist_append_nodup(klist *list, char *data)
{
    klist *last;
    klist *new_item;

    new_item = (klist *)malloc(sizeof(klist));
    if (!new_item)
        return NULL;

    new_item->next = NULL;
    new_item->data = data;

    // if this is the first item, then new_item is the list
    if (!list)
        return new_item;

    last = klist_last(list);
    last->next = new_item;

    return list;
}

/**
 * @brief Appends a string to the linked list. It always returns 
 * the address of the first record, so that you can use this function
 * as an initialization function as well as an append function.
 * 
 * @param list      The linked list, set NULL if first use
 * @param data      The string to append
 * @return klist*   Return pointed to first record of linked list
 */
klist *klist_append(klist *list, const char *data)
{
    char *dup_data = strdup(data);
    if (!dup_data)
        return NULL;

    list = klist_append_nodup(list, dup_data);
    if (!list)
        free(dup_data);

    return list;
}

/**
 * @brief Duplicates a linked list. It always returns the 
 * address of the first record of the cloned list or NULL 
 * in case of an error (or if the input list was NULL).
 * 
 * @param list      pointer to the list to duplicate
 * @return klist*   A pointer to newly allocated list or 
 * NULL if an error occurred.
 */
klist *klist_dup(klist *list)
{
    klist *out_list = NULL;
    klist *tmp;

    while (list) {
        tmp = klist_append(out_list, list->data);
        if (!tmp) {
            klist_free_all(out_list);
            return NULL;
        }

        out_list = tmp;
        list = list->next;
    }

    return out_list;
}

/**
 * @brief free a previously built klist
 * 
 * @param list 
 */
void klist_free_all(klist *list)
{
    klist *next;
    klist *item;

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