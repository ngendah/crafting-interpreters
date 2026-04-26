#pragma once

#include "common.h"
#include "value.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  size_t count;
  size_t capacity;
  byte_t *code;
  line_t *lines;
  values_t constants;
} chunk_t;

void chunk_init(chunk_t *const chunk);
void chunk_free(chunk_t *const chunk);
offset_t chunk_write(chunk_t *const chunk, byte_t byte, line_t line);
offset_t chunk_add_constant(chunk_t *const chunk, value_t value);
offset_t chunk_length(chunk_t *const chunk);
bool chunk_is_empty(chunk_t *const chunk);
void chunk_code_set_at(chunk_t *const chunk, offset_t at, byte_t byte);
