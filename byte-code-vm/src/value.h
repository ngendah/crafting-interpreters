#pragma once

#include <stddef.h>
#include <stdint.h>

#include "string.h"

typedef enum {
  VAL_BOOL = 0x01,
  VAL_NIL,
  VAL_NUMBER,
  VAL_STRING,
  VAL_OBJECT,
} value_type;

typedef struct {
  value_type type;
  union {
    bool boolean;
    double number;
    string_t string;
    object_t *object;
  } as;
} value_t;

typedef struct {
  size_t capacity;
  int count;
  value_t *values;
} values_t;

void values_init(values_t *values);
void values_free(values_t *values);
void values_write(values_t *values, value_t value);

void value_print(value_t value);

bool value_is_bool(value_t value);
bool value_is_nil(value_t value);
bool value_is_number(value_t value);
bool value_is_object(value_t value);
bool value_is_string(value_t value);

value_t value_nil();
value_t value_from_bool(bool value);
value_t value_from_number(double number);
value_t value_from_object(object_t *object);
value_t value_from_string(const string_t str);

bool value_as_bool(value_t value);
double value_as_number(value_t value);
string_t value_as_string(value_t value);
object_t *value_as_object(value_t value);

value_t value_add(const value_t a, value_t b);
