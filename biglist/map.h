#pragma once
#include <stdint.h>
#include <stdlib.h>
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct MapNode MapNode;

struct MapNode {
    uint32_t key;
    MapNode *last;
    MapNode *next;
};

typedef struct {
    int content_sizeof;
    MapNode *tail_nodes[(uint16_t)~0 + 1];
} Map;

static inline void map_make(int content_sizeof,
                            Map **out_map);

static inline void map_get(Map *map,
                           uint32_t key,
                           void **out_value);

static inline void map_add(Map *map,
                           uint32_t key,
                           void **out_value);

static inline void map_remove(Map *map,
                              uint32_t key);

static inline void map_slow_clear(Map *map);

static inline void map_slow_iter_head(Map *map,
                                      void **out_value,
                                      uint32_t *out_key);

static inline void map_slow_iter_next(Map *map,
                                      void *value,
                                      void **out_value,
                                      uint32_t *out_key);

static inline uint16_t map_hash(uint32_t key);


static inline void map_make(int content_sizeof,
                            Map **out_map)
{
    *out_map = calloc(1, sizeof(Map));
    (*out_map)->content_sizeof = content_sizeof;
}

static inline void map_get(Map *map,
                           uint32_t key,
                           void **out_value)
{
    MapNode *node = map->tail_nodes[map_hash(key)];

    while(node && node->key != key)
        node = node->last;

    *out_value = NULL;
    if (node)
        *out_value = offset(node, sizeof(MapNode));
}

static inline void map_add(Map *map,
                           uint32_t key,
                           void **out_value)
{
    int hash = map_hash(key);
    MapNode *tail = map->tail_nodes[hash];

    map->tail_nodes[hash] = calloc(1, sizeof(MapNode) + map->content_sizeof);
    map->tail_nodes[hash]->key  = key;
    map->tail_nodes[hash]->last = tail;

    if (tail)
        tail->next = map->tail_nodes[hash];

    *out_value = offset(map->tail_nodes[hash], sizeof(MapNode));
}

static inline void map_remove(Map *map,
                              uint32_t key)
{
    MapNode *node = map->tail_nodes[map_hash(key)];
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

static inline void map_slow_clear(Map *map)
{
    void *value;
    uint32_t key_curr;
    uint32_t key_last;
    map_slow_iter_head(map, &value, &key_curr);
    while (value) {
        key_last = key_curr;
        map_slow_iter_next(map, value, &value, &key_curr);

        /* We must remove after map_slow_iter_next() because map_remove() necessarily 
         * frees node, and map_slow_iter_next() needs node->last and node->key. */
        map_remove(map, key_last);
    }

    map_remove(map, key_curr);
}

static inline void map_slow_iter_head(Map *map,
                                      void **out_value,
                                      uint32_t *out_key)
{
    *out_value = NULL;
    for (int i = 0; i < (uint16_t)~0 + 1 && !*out_value; ++i)
        *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key   = ((MapNode *)*out_value)->key;
        *out_value = offset(*out_value, sizeof(MapNode));
    }
}

static inline void map_slow_iter_next(Map *map,
                                      void *value,
                                      void **out_value,
                                      uint32_t *out_key)
{
    MapNode *node = offset(value, -sizeof(MapNode));
    uint16_t hash = map_hash(node->key);

    *out_value = node->last;
    if (!*out_value)
        for (int i = hash + 1; i <= (uint16_t)~0 && !*out_value; ++i)
            *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key   = ((MapNode *)*out_value)->key;
        *out_value = offset(*out_value, sizeof(MapNode));
    }
}

static inline uint16_t map_hash(uint32_t key)
{
    return key * 2654435761 >> 16;
}