#pragma once
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "floor_stack.h"
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct {
    bool live;
    void *pre_content;
} fluid_list_EntryHead;

typedef struct {
    int capacity;
    int content_sizeof;
    floor_stack_Stack *add_stack;
} fluid_list_ListHead;

static inline void fluid_list_make(int content_sizeof,
                                   int capacity,
                                   fluid_list_ListHead **out_list);

static inline void fluid_list_head(fluid_list_ListHead *list,
                                   void **out_content);

static inline void fluid_list_next(fluid_list_ListHead *list,
                                   void *content,
                                   void **out_content);

static inline void fluid_list_add(fluid_list_ListHead *list,
                                  void **out_content);

static inline void fluid_list_remove(fluid_list_ListHead *list,
                                     void *content);

static inline void fluid_list_clear(fluid_list_ListHead *list);

static inline void fluid_list_make(int content_sizeof,
                                   int capacity,
                                   fluid_list_ListHead **out_list)
{
    *out_list = calloc(1, sizeof(fluid_list_ListHead)
        + capacity * (sizeof(fluid_list_EntryHead) + content_sizeof));
    (*out_list)->capacity       = capacity;
    (*out_list)->content_sizeof = content_sizeof;

    fluid_list_EntryHead **head;
    floor_stack_make(sizeof(fluid_list_EntryHead *),
                     &(*out_list)->add_stack, (void **)&head);
    *head = offset(*out_list, sizeof(fluid_list_ListHead));
    (*head)->live        = true;
    (*head)->pre_content = NULL;
}

static inline void fluid_list_head(fluid_list_ListHead *list,
                                   void **out_content)
{
    fluid_list_EntryHead *entry = offset(list, sizeof(fluid_list_ListHead));
    while (!entry->live)
        entry = offset(entry, sizeof(fluid_list_EntryHead) + list->content_sizeof);
    *out_content = entry->pre_content;
}

static inline void fluid_list_next(fluid_list_ListHead *list,
                                   void *content,
                                   void **out_content)
{
    fluid_list_EntryHead *entry = offset(content, list->content_sizeof);
    while (!entry->live)
        entry = offset(entry, sizeof(fluid_list_EntryHead) + list->content_sizeof);
    *out_content = entry->pre_content;
}

static inline void fluid_list_add(fluid_list_ListHead *list,
                                  void **out_content)
{
    fluid_list_EntryHead **entry;
    floor_stack_peak(list->add_stack, (void **)&entry);

    (*entry)->live        = true;
    (*entry)->pre_content = offset(*entry, sizeof(fluid_list_EntryHead));
    *out_content = offset(*entry, sizeof(fluid_list_EntryHead));

    /* We increment here because, in case we are currently at the floor,
     * the next available space would be the following entry. Otherwise,
     * the pop would revert this change, and we've saved ourselves a
     * possible branch. */
    *entry = offset(*entry, sizeof(fluid_list_EntryHead) + list->content_sizeof);
    floor_stack_pop_maybe(list->add_stack);

    floor_stack_peak_floor(list->add_stack, (void **)&entry);
    (*entry)->live        = true;
    (*entry)->pre_content = NULL;
}

static inline void fluid_list_remove(fluid_list_ListHead *list,
                                     void *content)
{
    fluid_list_EntryHead **entry;

    floor_stack_push(list->add_stack, (void **)&entry);
    *entry = offset(content, -sizeof(fluid_list_EntryHead));
    (*entry)->live        = false;
    (*entry)->pre_content = NULL;

    floor_stack_peak_floor(list->add_stack, (void **)&entry);
    (*entry)->live        = true;
    (*entry)->pre_content = NULL;
}

static inline void fluid_list_clear(fluid_list_ListHead *list)
{
    memset(offset(list, sizeof(fluid_list_ListHead)), 0, list->capacity 
        * (sizeof(fluid_list_EntryHead) + list->content_sizeof));
}