#include <assert.h>
#include <stdlib.h>

#include "memory.h"
#include "value.h"

void test_values_init() {
  values_t values;
  values_init(&values);
  assert(values.capacity == MIN_ARRAY_LEN);
  assert(values.count == 0);
}

void test_values_free() {
  values_t values;
  values_init(&values);
  assert(values.capacity == MIN_ARRAY_LEN);
  assert(values.count == 0);
  values_free(&values);
  assert(values.capacity == 0);
  assert(values.count == 0);
}

void test_value_bool() {
  const value_t value = value_from_bool(false);
  assert(value_is_bool(value) == true);
  assert(value_as_bool(value) == false);
}

void test_value_number() {
  const value_t value = value_from_number(1.2);
  assert(value_is_number(value) == true);
  assert(value_as_number(value) == 1.2);
}

void test_value_object() {
  object_t object = {};
  const value_t value = value_from_object(&object);
  assert(value_is_object(value) == true);
  assert(value_as_object(value) == &object);
}

void test_value_add() {
  const auto a = _("Hello ");
  const auto b = _("world!");
  const auto va = value_from_string(a);
  const auto vb = value_from_string(b);
  const auto result = value_add(va, vb);
  assert(string_compare(value_as_string(result), _("Hello world!")) == 0);
}

int main() {
  test_values_init();
  test_values_free();
  test_value_bool();
  test_value_number();
  test_value_object();
  test_value_add();
  return EXIT_SUCCESS;
}
