#pragma once
#include "list.h"
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct {
    uint32_t check_key;
    bool live;
} atom_list_EntryHead;

typedef struct {
    uint32_t next_check_key;
    list_ListHead *list;
} atom_list_List;

typedef struct {
    uint32_t check_key;
    atom_list_EntryHead *entry;
} atom_list_Atom;

static inline void atom_list_make(int content_sizeof,
                                  int capacity,
                                  atom_list_List **out_list);

static inline void atom_list_head(atom_list_List *list,
                                  void **out_content);

static inline void atom_list_next(atom_list_List *list,
                                  void *content,
                                  void **out_content);

static inline void atom_list_add(atom_list_List *list,
                                 void **out_content);

static inline void atom_list_remove(atom_list_List *list,
                                    void *content);

static inline void atom_list_get_atom(atom_list_List *list,
                                      void *content,
                                      atom_list_Atom *out_atom);

static inline void atom_list_get_item_maybe(atom_list_List *list,
                                            atom_list_Atom atom,
                                            void **out_content);

static inline void atom_list_make(int content_sizeof,
                                  int capacity,
                                  atom_list_List **out_list)
{
    *out_list = malloc(sizeof(atom_list_List));

    list_make(sizeof(atom_list_EntryHead) + content_sizeof,
              capacity,
              &(*out_list)->list);
}

static inline void atom_list_head(atom_list_List *list,
                                  void **out_content)
{
    list_head(list->list, out_content);
    *out_content = offset(*out_content, sizeof(atom_list_EntryHead));
}

static inline void atom_list_next(atom_list_List *list,
                                  void *content,
                                  void **out_content)
{
    void *entry = offset(content, -sizeof(atom_list_EntryHead));
    list_next(list->list, entry, out_content);
    *out_content = offset(*out_content, sizeof(atom_list_EntryHead));
}

static inline void atom_list_add(atom_list_List *list,
                                 void **out_content)
{
    atom_list_EntryHead *entry;
    list_add(list->list, (void **)&entry);
    entry->check_key = list->next_check_key++;
    entry->live      = true;

    *out_content = offset(entry, sizeof(atom_list_EntryHead));
}

static inline void atom_list_remove(atom_list_List *list,
                                    void *content)
{
    atom_list_EntryHead *entry = offset(content, -sizeof(atom_list_EntryHead));
    entry->live = false;
    list_remove(list->list, entry);
}

static inline void atom_list_get_atom(atom_list_List *list,
                                      void *content,
                                      atom_list_Atom *out_atom)
{
    atom_list_EntryHead *entry = offset(content, -sizeof(atom_list_EntryHead));
    out_atom->check_key = entry->check_key;
    out_atom->entry     = entry;
}

static inline void atom_list_get_item_maybe(atom_list_List *list,
                                            atom_list_Atom atom,
                                            void **out_content)
{
    *out_content = NULL;
    if (atom.entry->live && atom.entry->check_key == atom.check_key)
        *out_content = offset(atom.entry, sizeof(atom_list_EntryHead));
}