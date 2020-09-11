#pragma once
#include <stdint.h>
#include <stdlib.h>
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct map_NodeHead map_NodeHead;

struct map_NodeHead {
    uint32_t key;
    map_NodeHead *last;
    map_NodeHead *next;
};

typedef struct {
    int content_sizeof;
    map_NodeHead *tail_nodes[(uint16_t)~0 + 1];
} map_Map;

static inline void map_make(int content_sizeof,
                            map_Map **out_map);

static inline void map_get(map_Map *map,
                           uint32_t key,
                           void **out_value);

static inline void map_add(map_Map *map,
                           uint32_t key,
                           void **out_value);

static inline void map_remove(map_Map *map,
                              uint32_t key);


static inline void map_slow_iter_head(map_Map *map,
                                      void **out_value,
                                      uint32_t *out_key);

static inline void map_slow_iter_next(map_Map *map,
                                      void *value,
                                      void **out_value,
                                      uint32_t *out_key);

static inline uint16_t map_hash(uint32_t key);


static inline void map_make(int content_sizeof,
                            map_Map **out_map)
{
    *out_map = calloc(1, sizeof(map_Map));
    (*out_map)->content_sizeof = content_sizeof;
}

static inline void map_get(map_Map *map,
                           uint32_t key,
                           void **out_value)
{
    map_NodeHead *node = map->tail_nodes[map_hash(key)];

    while(node && node->key != key)
        node = node->last;

    *out_value = NULL;
    if (node)
        *out_value = offset(node, sizeof(map_NodeHead));
}

static inline void map_add(map_Map *map,
                           uint32_t key,
                           void **out_value)
{
    int hash = map_hash(key);
    map_NodeHead *tail = map->tail_nodes[hash];

    map->tail_nodes[hash] = calloc(1, sizeof(map_NodeHead) + map->content_sizeof);
    map->tail_nodes[hash]->key  = key;
    map->tail_nodes[hash]->last = tail;

    if (tail)
        tail->next = map->tail_nodes[hash];

    *out_value = offset(map->tail_nodes[hash], sizeof(map_NodeHead));
}

static inline void map_remove(map_Map *map,
                              uint32_t key)
{
    map_NodeHead *node = map->tail_nodes[map_hash(key)];
    map->tail_nodes[map_hash(key)] = NULL;

    while (node) {
        if (node->key == key) {
            if (node->last) node->last->next = node->next;
            if (node->next) node->next->last = node->last;
            free(node);

            return;
        }

        node = node->last;
    }
}


static inline void map_slow_iter_head(map_Map *map,
                                      void **out_value,
                                      uint32_t *out_key)
{
    *out_value = NULL;
    for (int i = 0; i < (uint16_t)~0 + 1 && !*out_value; ++i)
        *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key   = ((map_NodeHead *)*out_value)->key;
        *out_value = offset(*out_value, sizeof(map_NodeHead));
    }
}

static inline void map_slow_iter_next(map_Map *map,
                                      void *value,
                                      void **out_value,
                                      uint32_t *out_key)
{
    map_NodeHead *node = offset(value, -sizeof(map_NodeHead));
    uint16_t hash = map_hash(node->key);

    *out_value = node->last;
    if (!*out_value)
        for (int i = hash + 1; i <= (uint16_t)~0 && !*out_value; ++i)
            *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key   = ((map_NodeHead *)*out_value)->key;
        *out_value = offset(*out_value, sizeof(map_NodeHead));
    }
}

static inline uint16_t map_hash(uint32_t key)
{
    return key * 2654435761 >> 16;
}