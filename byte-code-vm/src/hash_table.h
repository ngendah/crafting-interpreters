#pragma once

#include "value.h"

typedef struct {
  hash_t key;
  value_t *value;
} hash_entry_t;

typedef void (*hash_entry_value_free)(value_t *value);

typedef struct hash_table_t {
  size_t capacity;
  size_t count;
  hash_entry_t *entries;
  hash_entry_value_free value_free;
} hash_table_t;

void hash_table_init(hash_table_t *table, hash_entry_value_free value_free);
void hash_table_free(hash_table_t *table);
void hash_table_add(hash_table_t *table, hash_entry_t entry);
value_t *hash_table_get_value(hash_table_t *table, hash_t key);
hash_entry_t *hash_table_find(hash_table_t *table, hash_t key);
void hash_table_delete(hash_table_t *table, hash_t key);
