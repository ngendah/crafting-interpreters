#pragma once

#include "common.h"

typedef struct {
  object_t object;
  const char *str;
  size_t length;
} string_t;

const string_t _(const char *str);
const string_t _T(const char *str, size_t length);

const int string_compare(const string_t a, const string_t b);
const double string_as_double(string_t str);
