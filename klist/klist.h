/**
 * @file klist.h
 * @author songjian (kyosold@qq.com)
 * @brief   a linked list
 * @version 0.1
 * @date 2022-12-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __K_LIST_H__
#define __K_LIST_H__

// linked-list structure
typedef struct klist_st klist;
struct klist_st
{
    char *data;
    klist *next;
};

klist *klist_append(klist *list, const char *data);

void klist_free_all(klist *list);

klist *klist_dup(klist *list);

#endif
