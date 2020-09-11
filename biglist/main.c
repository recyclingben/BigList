#include "list.h"

int main()
{
    Table *table;
    table_make(sizeof(int), 10, &table);

    int *value;
    table_add(table, &value); *value = 1;
    table_add(table, &value); *value = 2;
    table_add(table, &value); *value = 3;
    table_add(table, &value); *value = 4;
    table_clear(table);
    table_add(table, &value); *value = 1;
    table_add(table, &value); *value = 2;
    table_add(table, &value); *value = 3;

    table_head(table, &value);
    while (value) {
        printf("%d\n", *value);
        table_next(table, value, &value);
    }
}