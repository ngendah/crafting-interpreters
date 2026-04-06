#pragma once

#include "common.h"

typedef struct {
  object_t object;
  const char *str;
  size_t length;
} string_t;

constexpr auto null_string = (string_t){
    .length = 0, .str = nullptr, .object = {.hash = 0, .is_marked = false}};

const string_t _(const char str[]);
const string_t _T(const char *str, size_t length);
const string_t _T_M(const char *str, size_t length, bool is_marked);

const int string_compare(const string_t a, const string_t b);
const double string_as_double(string_t str);
