#pragma once
#include "fluid_list.h"
#include "map.h"
#include "map2d.h"

typedef fluid_list_ListHead table_Table;
typedef map_Map             index_Index;
typedef map2d_Map           index_Index2D;

#define table_make(content_sizeof, capacity, out_table) fluid_list_make(content_sizeof, capacity, out_table)
#define table_head(table, out_content)                  fluid_list_head(table, (void **)(out_content))
#define table_next(table, content, out_content)         fluid_list_next(table, content, (void **)(out_content))
#define table_add(table, out_content)                   fluid_list_add(table, (void **)(out_content))
#define table_remove(table, content)                    fluid_list_remove(table, content)
#define table_clear(table)                              fluid_list_clear(table)

#define index_make(content_sizeof, out_index)                      map_make(content_sizeof, out_index)
#define index_get(index, key, out_content)                         map_get(index, key, (void **)out_content)
#define index_add(index, key, out_content)                         map_add(index, key, (void **)out_content)
#define index_remove(index, key)                                   map_remove(index, key)
#define index_slow_clear(index)                                    map_slow_clear(index)
#define index_slow_iter_head(index, out_content, out_key)          map_slow_iter_head(index, (void **)out_content, out_key)
#define index_slow_iter_next(index, content, out_content, out_key) map_slow_iter_next(index, content, (void **)out_content, out_key)

#define index2d_make(content_size, out_index)                                     map2d_make(content_size, out_index)
#define index2d_get(index, key_x, key_y, out_content)                             map2d_get(index, key_x, key_y, (void **)(out_content))
#define index2d_add(index, key_x, key_y, out_content)                             map2d_add(index, key_x, key_y, (void **)(out_content))
#define index2d_remove(index, key_x, key_y)                                       map2d_remove(index, key_x, key_y)
#define index2d_slow_clear(index)                                                 map2d_slow_clear(index)
#define index2d_slow_iter_head(index, out_content, out_key_x, out_key_y)          map2d_slow_iter_head(index, (void **)out_content, out_key_x, out_key_y)
#define index2d_slow_iter_next(index, content, out_content, out_key_x, out_key_y) map2d_slow_iter_next(index, content, (void **)out_content, out_key_x, out_key_y)