#pragma once
#include <stdint.h>
#include <stdlib.h>
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct Map2DNode Map2DNode;

struct Map2DNode {
    uint32_t key_x;
    uint32_t key_y;
    Map2DNode *last;
    Map2DNode *next;
};

typedef struct {
    int content_sizeof;
    Map2DNode *tail_nodes[(uint16_t)~0 + 1];
} Map2D;

static inline void map2d_make(int content_sizeof,
                              Map2D **out_map);

static inline void map2d_get(Map2D *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value);

static inline void map2d_add(Map2D *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value);

static inline void map2d_remove(Map2D *map,
                                uint32_t key_x,
                                uint32_t key_y);

static inline void map2d_slow_iter_head(Map2D *map,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y);

static inline void map2d_slow_clear(Map2D *map);

static inline void map2d_slow_iter_next(Map2D *map,
                                        void *value,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y);

static inline uint16_t map2d_hash(uint32_t key_x,
                                  uint32_t key_y);


static inline void map2d_make(int content_sizeof,
                              Map2D **out_map)
{
    *out_map = calloc(1, sizeof(Map2D));
    (*out_map)->content_sizeof = content_sizeof;
}

static inline void map2d_get(Map2D *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value)
{
    Map2DNode *node = map->tail_nodes[map2d_hash(key_x, key_y)];

    while(node && (node->key_x != key_x || node->key_y != key_y))
        node = node->last;

    *out_value = NULL;
    if (node)
        *out_value = offset(node, sizeof(Map2DNode));
}

static inline void map2d_add(Map2D *map,
                             uint32_t key_x,
                             uint32_t key_y,
                             void **out_value)
{
    int hash = map2d_hash(key_x, key_y);
    Map2DNode *tail = map->tail_nodes[hash];

    map->tail_nodes[hash] = malloc(sizeof(Map2DNode) + map->content_sizeof);
    map->tail_nodes[hash]->key_x = key_x;
    map->tail_nodes[hash]->key_y = key_y;
    map->tail_nodes[hash]->last  = tail;

    if (tail)
        tail->next = map->tail_nodes[hash];

    *out_value = offset(map->tail_nodes[hash], sizeof(Map2DNode));
}

static inline void map2d_remove(Map2D *map,
                                uint32_t key_x,
                                uint32_t key_y)
{
    Map2DNode *node = map->tail_nodes[map2d_hash(key_x, key_y)];

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

static inline void map2d_slow_clear(Map2D *map)
{
    void *value;
    uint32_t key_x_curr;
    uint32_t key_y_curr;
    uint32_t key_x_last;
    uint32_t key_y_last;
    map2d_slow_iter_head(map, &value, &key_x_curr, &key_y_curr);
    while (value) {
        key_x_last = key_x_curr;
        key_y_last = key_y_curr;
        map2d_slow_iter_next(map, value, &value, &key_x_curr, &key_y_curr);

        /* See map_slow_clear() for why we remove after getting the next. */
        map2d_remove(map, key_x_last, key_y_last);
    }
}

static inline void map2d_slow_iter_head(Map2D *map,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y)
{
    *out_value = NULL;
    for (int i = 0; i < (uint16_t)~0 + 1 && !*out_value; ++i)
        *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key_x = ((Map2DNode *)*out_value)->key_x;
        *out_key_y = ((Map2DNode *)*out_value)->key_y;
        *out_value = offset(*out_value, sizeof(Map2DNode));
    }
}

static inline void map2d_slow_iter_next(Map2D *map,
                                        void *value,
                                        void **out_value,
                                        uint32_t *out_key_x,
                                        uint32_t *out_key_y)
{
    Map2DNode *node = offset(value, -sizeof(Map2DNode));
    uint16_t hash = map2d_hash(node->key_x, node->key_y);

    *out_value = node->last;
    if (!*out_value)
        for (int i = hash + 1; i < (uint16_t)~0 + 1 && !*out_value; ++i)
            *out_value = map->tail_nodes[i];

    if (*out_value) {
        *out_key_x = ((Map2DNode *)*out_value)->key_x;
        *out_key_y = ((Map2DNode *)*out_value)->key_y;
        *out_value = offset(*out_value, sizeof(Map2DNode));
    }
}

static inline uint16_t map2d_hash(uint32_t key_x,
                                  uint32_t key_y)
{
    return (key_x * 2654435761 >> 16) ^ (key_y * 2654435761 >> 16);
}