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
} FluidListEntry;

typedef struct {
    int capacity;
    int content_sizeof;
    FloorStack *add_stack;
} FluidList;

static inline void fluid_list_make(int content_sizeof,
                                   int capacity,
                                   FluidList **out_list);

static inline void fluid_list_head(FluidList *list,
                                   void **out_content);

static inline void fluid_list_next(FluidList *list,
                                   void *content,
                                   void **out_content);

static inline void fluid_list_add(FluidList *list,
                                  void **out_content);

static inline void fluid_list_remove(FluidList *list,
                                     void *content);

static inline void fluid_list_clear(FluidList *list);

static inline void fluid_list_make(int content_sizeof,
                                   int capacity,
                                   FluidList **out_list)
{
    *out_list = calloc(1, sizeof(FluidList)
        + capacity * (sizeof(FluidListEntry) + content_sizeof));
    (*out_list)->capacity       = capacity;
    (*out_list)->content_sizeof = content_sizeof;

    FluidListEntry **head;
    floor_stack_make(sizeof(FluidListEntry *), &(*out_list)->add_stack, (void **)&head);
    *head = offset(*out_list, sizeof(FluidList));
    (*head)->live        = true;
    (*head)->pre_content = NULL;
}

static inline void fluid_list_head(FluidList *list,
                                   void **out_content)
{
    FluidListEntry *entry = offset(list, sizeof(FluidList));
    while (!entry->live)
        entry = offset(entry, sizeof(FluidListEntry) + list->content_sizeof);
    *out_content = entry->pre_content;
}

static inline void fluid_list_next(FluidList *list,
                                   void *content,
                                   void **out_content)
{
    FluidListEntry *entry = offset(content, list->content_sizeof);
    while (!entry->live)
        entry = offset(entry, sizeof(FluidListEntry) + list->content_sizeof);
    *out_content = entry->pre_content;
}

static inline void fluid_list_add(FluidList *list,
                                  void **out_content)
{
    FluidListEntry **entry;
    floor_stack_peak(list->add_stack, (void **)&entry);

    (*entry)->live        = true;
    (*entry)->pre_content = offset(*entry, sizeof(FluidListEntry));
    *out_content = offset(*entry, sizeof(FluidListEntry));

    /* We increment here because, in case we are currently at the floor,
     * the next available space would be the following entry. Otherwise,
     * the pop would revert this change, and we've saved ourselves a
     * possible branch. */
    *entry = offset(*entry, sizeof(FluidListEntry) + list->content_sizeof);
    floor_stack_pop_maybe(list->add_stack);

    floor_stack_peak_floor(list->add_stack, (void **)&entry);
    (*entry)->live        = true;
    (*entry)->pre_content = NULL;
}

static inline void fluid_list_remove(FluidList *list,
                                     void *content)
{
    FluidListEntry **entry;

    floor_stack_push(list->add_stack, (void **)&entry);
    *entry = offset(content, -sizeof(FluidListEntry));
    (*entry)->live        = false;
    (*entry)->pre_content = NULL;

    floor_stack_peak_floor(list->add_stack, (void **)&entry);
    (*entry)->live        = true;
    (*entry)->pre_content = NULL;
}

static inline void fluid_list_clear(FluidList *list)
{
    memset(offset(list, sizeof(FluidList)), 0, list->capacity 
        * (sizeof(FluidListEntry) + list->content_sizeof));
    
    FluidListEntry **head;
    floor_stack_clear(list->add_stack, (void **)&head);
    *head = offset(list, sizeof(FluidList));
    (*head)->live        = true;
    (*head)->pre_content = NULL;
}