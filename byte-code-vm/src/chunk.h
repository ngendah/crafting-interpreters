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

void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);
void chunk_write(chunk_t *chunk, byte_t byte, line_t line);
offset_t chunk_add_constant(chunk_t *chunk, value_t value);
