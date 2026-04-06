#include <assert.h>

#include "common.h"
#include "stack.h"

void stack_init(stack_t *stack) { stack->top = 0; }

void stack_free(stack_t *stack) { stack->top = 0; }

bool stack_is_empty(stack_t *stack) { return stack->top == 0; }

offset_t stack_push(stack_t *stack, value_t value) {
  assert(stack->top + 1 < STACK_SIZE);
  stack->values[stack->top++] = value;
  return stack->top - 1;
}

value_t stack_pop(stack_t *stack) {
  assert(!stack_is_empty(stack));
  return stack->values[--stack->top];
}

value_t stack_peek(stack_t *stack) {
  assert(!stack_is_empty(stack));
  auto const top = stack->top - 1;
  return stack->values[top];
}

value_t stack_peek_at(stack_t *stack, offset_t at) {
  assert(!stack_is_empty(stack));
  auto const index = stack->top - 1 - at;
  return stack->values[index];
}

value_t stack_set_at(stack_t *stack, offset_t at, value_t value) {
  assert(!stack_is_empty(stack));
  auto const index = stack->top - 1 - at;
  auto const prev_value = stack->values[index];
  stack->values[index] = value;
  return prev_value;
}
