#include "hash_table.h"
#include "memory.h"
#include "string.h"
#include "value.h"
#include <assert.h>
#include <stdlib.h>

void test_hash_table_init() {
  hash_table_t table;
  hash_table_init(&table, nullptr);
  assert(table.count == 0);
  assert(table.capacity == MIN_ARRAY_LEN);
  assert(table.entries != nullptr);
  hash_table_free(&table);
}

void test_hash_table_add() {
  hash_table_t table;
  hash_table_init(&table, nullptr);
  {
    auto str = _("test str");
    auto value = value_from_string(str, nullptr);
    hash_table_add(&table,
                   (hash_entry_t){.key = str.object.hash, .value = value});
  }
  assert(table.count == 1);
  // FIXME: free value
  hash_table_free(&table);
}

void test_hash_table_get_value() {
  hash_table_t table;
  hash_table_init(&table, nullptr);
  hash_t find_key = 0;
  auto str1 = _("test str");
  find_key = str1.object.hash;
  auto value1 = value_from_string(str1, nullptr);
  hash_table_add(&table,
                 (hash_entry_t){.key = str1.object.hash, .value = value1});
  auto str2 = _("test another str");
  auto value2 = value_from_string(str2, nullptr);
  hash_table_add(&table,
                 (hash_entry_t){.key = str2.object.hash, .value = value2});
  assert(table.count == 2);
  const value_t *value = hash_table_get_value(&table, find_key);
  assert(value != nullptr);
  assert(value_is_string(*value));
  assert(string_compare(value_as_string(*value), _("test str")) == 0);
  // FIXME: free value
  hash_table_free(&table);
}

int main() {
  test_hash_table_init();
  test_hash_table_add();
  test_hash_table_get_value();
  return EXIT_SUCCESS;
}
