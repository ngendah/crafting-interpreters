#pragma once

#include "common.h"
#include "value.h"

// TODO: convert to a general stack

constexpr auto STACK_SIZE = 256u;

typedef struct {
  value_t values[STACK_SIZE];
  offset_t top;
} stack_t;

void stack_init(stack_t *stack);
void stack_free(stack_t *stack);
bool stack_is_empty(stack_t *stack);
offset_t stack_push(stack_t *stack, value_t value);
value_t stack_pop(stack_t *stack);
value_t stack_peek(stack_t *stack);
value_t stack_peek_at(stack_t *stack, offset_t at);
value_t stack_set_at(stack_t *stack, offset_t at, value_t value);
