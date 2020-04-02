#pragma once
#include <stdint.h>
#include "list.h"

typedef struct map_NodeHead map_NodeHead;

struct map_NodeHead {
    uint32_t key;
    map_NodeHead *last;
    map_NodeHead *next;
};

typedef struct {
    int content_sizeof;
    map_NodeHead *tail_nodes[(uint16_t)~0];
} map_Map;

void map_make(int content_sizeof,
              map_Map **out_map);

void map_get(map_Map *map,
             uint32_t key,
             void **out_value);

bool map_has(map_Map *map,
             uint32_t key);

void map_add(map_Map *map,
             uint32_t key,
             void **out_value);

void map_remove(map_Map *map,
                uint32_t key);

void map_remove_value(map_Map *map,
                      void *value);

uint16_t map_hash(uint32_t key);


void map_make(int content_sizeof,
              map_Map **out_map)
{
    *out_map = calloc(1, sizeof(map_Map));
    (*out_map)->content_sizeof = content_sizeof;
}

void map_get(map_Map *map,
             uint32_t key,
             void **out_value)
{
    map_NodeHead *node = map->tail_nodes[map_hash(key)];

    while(node->key != key) {
        node = node->last;
    }
    *out_value = offset(node, sizeof(map_NodeHead));
}

bool map_has(map_Map *map,
             uint32_t key)
{
    map_NodeHead *node = map->tail_nodes[map_hash(key)];

    while (node) {
        if (node->key == key)
            return true;

        node = node->last;
    }
    return false;
}

void map_add(map_Map *map,
             uint32_t key,
             void **out_value)
{
    int hash = map_hash(key);
    map_NodeHead *tail = map->tail_nodes[hash];

    map->tail_nodes[hash] = malloc(sizeof(map_NodeHead) + map->content_sizeof);
    map->tail_nodes[hash]->key  = key;
    map->tail_nodes[hash]->last = tail;

    if (tail)
        tail->next = map->tail_nodes[hash];

    *out_value = offset(map->tail_nodes[hash], sizeof(map_NodeHead));
}

void map_remove(map_Map *map,
                uint32_t key)
{
    map_NodeHead *node = map->tail_nodes[map_hash(key)];

    while (node) {
        if (node->key == key) {
            if (node->last)
                node->last->next = node->next;
            if (node->next)
                node->next->last = node->last;
            free(node);

            return;
        }

        node = node->last;
    }
}

void map_remove_value(map_Map *map,
                      void *value)
{
    map_NodeHead *node = offset(value, -sizeof(map_NodeHead));

    if (node->last)
        node->last->next = node->next;
    if (node->next)
        node->next->last = node->last;
    free(node);
}

uint16_t map_hash(uint32_t key)
{
    return key * 2654435761 >> 16;
}