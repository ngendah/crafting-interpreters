#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hash_table.h"
#include "memory.h"

constexpr float TABLE_MAX_LOAD = 0.75;
constexpr hash_t tombstone = ((hash_t)0) - 1;

void hash_table_init(hash_table_t *table, hash_entry_value_free value_free) {
  table->capacity = MIN_ARRAY_LEN;
  table->count = 0;
  size_t size = MIN_ARRAY_LEN * sizeof(hash_entry_t);
  table->entries = (hash_entry_t *)new(size);
  table->value_free = value_free;
  memset(table->entries, 0, size);
}

void hash_table_entries_free(hash_table_t *table) {
  if (table->value_free == nullptr)
    return;
  auto ptr = &table->entries[0];
  while (ptr != &table->entries[table->capacity]) {
    if (ptr->value != nullptr)
      table->value_free(ptr->value);
    ptr++;
  }
}

void hash_table_free(hash_table_t *table) {
  hash_table_entries_free(table);
  delete(table->entries);
  table->entries = nullptr;
  table->capacity = 0;
  table->count = 0;
}

void hash_table_extend(hash_table_t *table) {
  if (table->count + 1 < table->capacity) {
    return;
  }
  size_t capacity = table->capacity * TABLE_MAX_LOAD;
  // FIXME: capacity != max_value_of(hash_t)
  hash_table_t new_table = {.count = 0,
                            .capacity = capacity,
                            .entries = new(capacity * sizeof(hash_entry_t))};
  for (hash_entry_t *it = &table->entries[0];
       it != &table->entries[table->count]; it++) {
    hash_table_add(&new_table, *it);
  }
  delete(table->entries);
  table->capacity = new_table.capacity;
  table->count = new_table.count;
  table->entries = new_table.entries;
}

void hash_table_add(hash_table_t *table, hash_entry_t entry) {
  hash_table_extend(table);
  const auto ptr = hash_table_find(table, entry.key);
  if (ptr->key == 0)
    table->count++;
  ptr->key = entry.key;
  ptr->value = entry.value;
}

hash_entry_t *hash_table_find(hash_table_t *table, hash_t key) {
  auto index = key % (table->capacity);
  while (true) {
    // FIXME: re-use tombstone buckets
    auto entry = &(table->entries[index]);
    if (entry->key == 0 || entry->key == key) {
      return entry;
    }
    index = (index + 1) % table->capacity;
  }
}

value_t *hash_table_get_value(hash_table_t *table, hash_t key) {
  const auto entry = hash_table_find(table, key);
  return entry->value;
}

void hash_table_delete(hash_table_t *table, hash_t key) {
  const auto entry = hash_table_find(table, key);
  if (entry->key != 0 && entry->key != tombstone) {
    entry->key = tombstone;
    if (table->value_free)
      table->value_free(entry->value);
    entry->value = nullptr;
  }
}
