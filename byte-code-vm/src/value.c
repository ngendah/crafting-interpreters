#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "value.h"

void value_free(value_t);

void values_init(values_t *values) {
  values->values = (value_t *)new(MIN_ARRAY_LEN * sizeof(value_t));
  values->capacity = MIN_ARRAY_LEN;
  values->count = 0;
}

void values_write(values_t *values, value_t value) {
  if (values->count + 1 >= values->capacity) {
    size_t len = values->capacity * GROW_RATE;
    // BUG:
    value_t *ptr = (value_t *)resize(values->values, len * sizeof(value_t));
    values->values = ptr;
    values->capacity = len;
  }
  values->values[values->count] = value;
  values->count++;
}

void values_free(values_t *values) {
  value_t *ptr = values->values;
  while (ptr != &values->values[values->count]) {
    value_free(*ptr);
    ptr++;
  }
  delete(values->values);
  values->count = 0;
  values->capacity = 0;
  values->values = nullptr;
}

void value_free(value_t value) {
  if (value_is_string(value)) {
    delete((void *)value.as.string.str);
  }
}

void value_print(value_t value) {
  switch (value.type) {
  case VAL_NIL:
    printf("%s", "nil");
    break;
  case VAL_NUMBER:
    printf("%g", value_as_number(value));
    break;
  case VAL_BOOL:
    printf("%s", value_as_bool(value) ? "true" : "false");
    break;
  default:
    break;
  }
}

bool value_is_bool(value_t value) { return value.type == VAL_BOOL; }

bool value_is_nil(value_t value) { return value.type == VAL_NIL; }

bool value_is_number(value_t value) { return value.type == VAL_NUMBER; }

bool value_is_object(value_t value) { return value.type == VAL_OBJECT; }

bool value_is_string(value_t value) { return value.type == VAL_STRING; }

value_t value_nil() { return (value_t){.type = VAL_NIL, .as = {.number = 0}}; }

value_t value_from_bool(bool value) {
  return (value_t){.type = VAL_BOOL, .as = {.boolean = value}};
}

value_t value_from_number(double value) {
  return (value_t){.type = VAL_NUMBER, .as = {.number = value}};
}

value_t value_from_object(object_t *object) {
  return (value_t){.type = VAL_OBJECT, .as = {.object = object}};
}

value_t value_from_string(const string_t str) {
  const size_t len = str.length;
  char *ptr = (char *)new(len * sizeof(char));
  memset(ptr, '\0', len);
  strncpy(ptr, str.str, len);
  string_t nstr = {
      .object =
          {
              .hash = str.object.hash,
              .is_marked = false,
          },
      .str = ptr,
      .length = len,
  };
  return (value_t){.type = VAL_STRING, .as = {.string = nstr}};
}

double value_as_number(value_t value) { return value.as.number; }

bool value_as_bool(value_t value) { return value.as.boolean; }

object_t *value_as_object(value_t value) { return value.as.object; }

string_t value_as_string(value_t value) { return value.as.string; }

extern string_t string_concatenate(const string_t a, const string_t b);

value_t value_add(const value_t a, const value_t b) {
  assert(a.type == VAL_NUMBER || a.type == VAL_STRING);
  assert(a.type == b.type);
  if (a.type == VAL_NUMBER) {
    return value_from_number(value_as_number(a) + value_as_number(b));
  } else {
    const auto str = string_concatenate(value_as_string(a), value_as_string(b));
    return (value_t){.type = VAL_STRING, .as = {.string = str}};
  }
}
