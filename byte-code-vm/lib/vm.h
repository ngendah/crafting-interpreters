#pragma once

#include <stddef.h>
#include <stdint.h>

#include "chunk.h"
#include "common.h"
#include "environment.h"
#include "stack.h"

typedef enum : byte_t {
  OK = 0x00,
  COMPILE_ERROR,
  RUNTIME_ERROR,
} vm_result_t;

typedef struct {
  chunk_t *chunk;
  offset_t ip;
  stack_t stack;
  environment_t environment;
} vm_t;

void vm_init();
void vm_free();
vm_result_t vm_run(const string_t source);
