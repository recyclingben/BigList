#pragma once
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include "floor_stack.h"
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct {
    bool live;
    void *pre_content;
} list_EntryHead;

typedef struct {
    int content_sizeof;
    floor_stack_Stack *add_stack;
} list_List;

void list_make(int content_sizeof,
               int capacity,
               list_List **out_list);

void list_head(list_List *list,
               void **out_content);

void list_next(list_List *list,
               void *content,
               void **out_content);

void list_add(list_List *list,
              void **out_content);

void list_remove(list_List *list,
                 void *content);

void list_make(int content_sizeof,
               int capacity,
               list_List **out_list)
{
    *out_list = calloc(1, sizeof(list_List)
        + capacity * (sizeof(list_EntryHead) + content_sizeof));
    (*out_list)->content_sizeof = content_sizeof;

    list_EntryHead **head;
    floor_stack_make(sizeof(list_EntryHead *),
                     &(*out_list)->add_stack, (void **)&head);
    *head = offset(*out_list, sizeof(list_List));
    (*head)->live        = true;
    (*head)->pre_content = NULL;
}

void list_head(list_List *list,
               void **out_content)
{
    list_EntryHead *entry = offset(list, sizeof(list_List));
    while (!entry->live)
        entry = offset(entry, sizeof(list_EntryHead) + list->content_sizeof);
    *out_content = entry->pre_content;
}

void list_next(list_List *list,
               void *content,
               void **out_content)
{
    list_EntryHead *entry = offset(content, list->content_sizeof);
    while (!entry->live)
        entry = offset(entry, sizeof(list_EntryHead) + list->content_sizeof);
    *out_content = entry->pre_content;
}

void list_add(list_List *list,
              void **out_content)
{
    list_EntryHead **entry;
    floor_stack_peak(list->add_stack, (void **)&entry);

    (*entry)->live        = true;
    (*entry)->pre_content = offset(*entry, sizeof(list_EntryHead));
    *out_content = offset(*entry, sizeof(list_EntryHead));

    /* We increment here because, in case we are currently at the floor,
     * the next available space would be the following entry. Otherwise,
     * the pop would revert this change, and we've saved ourselves a
     * possible branch. */
    *entry = offset(*entry, sizeof(list_EntryHead) + list->content_sizeof);
    floor_stack_pop_maybe(list->add_stack);

    floor_stack_peak_floor(list->add_stack, (void **)&entry);
    (*entry)->live        = true;
    (*entry)->pre_content = NULL;
}

void list_remove(list_List *list,
                 void *content)
{
    list_EntryHead **entry;

    floor_stack_push(list->add_stack, (void **)&entry);
    *entry = offset(content, -sizeof(list_EntryHead));
    (*entry)->live        = false;
    (*entry)->pre_content = NULL;

    floor_stack_peak_floor(list->add_stack, (void **)&entry);
    (*entry)->live        = true;
    (*entry)->pre_content = NULL;
}