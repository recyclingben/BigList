#pragma once
#include <stdint.h>
#include <stdlib.h>
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct map2d_NodeHead map2d_NodeHead;

struct map2d_NodeHead {
    uint32_t key_x;
    uint32_t key_y;
    map2d_NodeHead *last;
    map2d_NodeHead *next;
};

typedef struct {
    int content_sizeof;
    map2d_NodeHead *tail_nodes[(uint16_t)~0 + 1];
} map2d_Map;

static inline void map2d_make(int content_sizeof,
                              map2d_Map **out_map);

static inline void map2d_get(map2d_Map *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value);

static inline void map2d_add(map2d_Map *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value);

static inline void map2d_remove(map2d_Map *map,
                                uint32_t key_x,
                                uint32_t key_y);

static inline void map2d_slow_iter_head(map2d_Map *map,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y);

static inline void map2d_slow_iter_next(map2d_Map *map,
                                        void *value,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y);

static inline uint16_t map2d_hash(uint32_t key_x,
                                  uint32_t key_y);


static inline void map2d_make(int content_sizeof,
                              map2d_Map **out_map)
{
    *out_map = calloc(1, sizeof(map2d_Map));
    (*out_map)->content_sizeof = content_sizeof;
}

static inline void map2d_get(map2d_Map *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value)
{
    map2d_NodeHead *node = map->tail_nodes[map2d_hash(key_x, key_y)];

    while(node && (node->key_x != key_x || node->key_y != key_y))
        node = node->last;

    *out_value = NULL;
    if (node)
        *out_value = offset(node, sizeof(map2d_NodeHead));
}

static inline void map2d_add(map2d_Map *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value)
{
    int hash = map2d_hash(key_x, key_y);
    map2d_NodeHead *tail = map->tail_nodes[hash];

    map->tail_nodes[hash] = malloc(sizeof(map2d_NodeHead) + map->content_sizeof);
    map->tail_nodes[hash]->key_x = key_x;
    map->tail_nodes[hash]->key_y = key_y;
    map->tail_nodes[hash]->last  = tail;

    if (tail)
        tail->next = map->tail_nodes[hash];

    *out_value = offset(map->tail_nodes[hash], sizeof(map2d_NodeHead));
}

static inline void map2d_remove(map2d_Map *map,
                                uint32_t key_x,
                                uint32_t key_y)
{
    map2d_NodeHead *node = map->tail_nodes[map2d_hash(key_x, key_y)];

    while (node) {
        if (node->key_x == key_x && node->key_y == key_y) {
            if (node->last) node->last->next = node->next;
            if (node->next) node->next->last = node->last;
            free(node);

            return;
        }

        node = node->last;
    }
}


static inline void map2d_slow_iter_head(map2d_Map *map,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y)
{
    *out_value = NULL;
    for (int i = 0; i < (uint16_t)~0 + 1 && !*out_value; ++i)
        *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key_x = ((map2d_NodeHead *)*out_value)->key_x;
        *out_key_y = ((map2d_NodeHead *)*out_value)->key_y;
        *out_value = offset(*out_value, sizeof(map2d_NodeHead));
    }
}

static inline void map2d_slow_iter_next(map2d_Map *map,
                                        void *value,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y)
{
    map2d_NodeHead *node = offset(value, -sizeof(map2d_NodeHead));
    uint16_t hash = map2d_hash(node->key_x, node->key_y);

    *out_value = node->last;
    if (!*out_value)
        for (int i = hash + 1; i < (uint16_t)~0 + 1 && !*out_value; ++i)
            *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key_x = ((map2d_NodeHead *)*out_value)->key_x;
        *out_key_y = ((map2d_NodeHead *)*out_value)->key_y;
        *out_value = offset(*out_value, sizeof(map2d_NodeHead));
    }
}

static inline uint16_t map2d_hash(uint32_t key_x,
                                  uint32_t key_y)
{
    return (key_x * 2654435761 >> 16) ^ (key_y * 2654435761 >> 16);
}