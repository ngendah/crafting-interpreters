#pragma once

#include <stddef.h>
#include <stdint.h>

#include "chunk.h"
#include "common.h"
#include "string.h"
#include "value.h"

typedef enum {
  OK = 0x00,
  COMPILE_ERROR,
  RUNTIME_ERROR,
} vm_result_t;

#define STACK_SIZE (size_t)256

typedef struct {
  value_t values[STACK_SIZE];
  offset_t top;
} stack_t;

typedef struct {
  chunk_t *chunk;
  offset_t ip;
  stack_t stack;
} vm_t;

void vm_init();
void vm_free();
vm_result_t vm_run(const string_t source);

void stack_init(stack_t *stack);
void stack_free(stack_t *stack);
bool stack_is_empty(stack_t *stack);
offset_t stack_push(stack_t *stack, value_t value);
value_t stack_pop(stack_t *stack);
value_t stack_peek(stack_t *stack);
value_t stack_peek_at(stack_t *stack, int at);
