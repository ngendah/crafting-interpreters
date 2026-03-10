#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "vm.h"
#ifdef DEBUG_TRACE_EXECUTION
#include "debug.h"
#endif
#include "value.h"

bool vm_is_end();
vm_result_t vm_execute();
byte_t vm_next_instruction();
value_t vm_read_constant();
void vm_add();
void vm_subtract();
void vm_multiply();
void vm_divide();
void vm_negate();
void vm_equal();
void vm_greater();
void vm_less();
void vm_runtime_error(const char *format, ...);

vm_t vm;

void vm_init() {
  vm.chunk = (chunk_t *)new(sizeof(chunk_t));
  vm.ip = 0;
  chunk_init(vm.chunk);
  stack_init(&vm.stack);
}

void vm_free() {
  chunk_free(vm.chunk);
  delete(vm.chunk);
  stack_free(&vm.stack);
  vm.chunk = nullptr;
  vm.ip = 0;
}

vm_result_t vm_run(const string_t source) {
  if (!compiler_run(source, vm.chunk)) {
    return COMPILE_ERROR;
  }
  return vm_execute();
}

bool value_can_add(const value_t a, const value_t b) {
  return ((value_is_number(a) && value_is_number(b)) ||
          (value_is_string(a) && value_is_string(b)));
}

vm_result_t vm_execute() {

#define CHECK_BINARY_OPERANDS                                                  \
  if (!value_is_number(stack_peek_at(&vm.stack, 0)) ||                         \
      !value_is_number(stack_peek_at(&vm.stack, 1))) {                         \
    vm_runtime_error("Operands must be numbers.");                             \
    return RUNTIME_ERROR;                                                      \
  }

#define CHECK_UNARY_OPERAND                                                    \
  if (!value_is_number(stack_peek_at(&vm.stack, 0))) {                         \
    vm_runtime_error("Operand must be a number.");                             \
    return RUNTIME_ERROR;                                                      \
  }

#define CHECK_NOT_OPERAND                                                      \
  if (!value_is_bool(stack_peek_at(&vm.stack, 0)) ||                           \
      !value_is_nil(stack_peek_at(&vm.stack, 0))) {                            \
    vm_runtime_error("Operand must be a boolean.");                            \
    return RUNTIME_ERROR;                                                      \
  }

  while (!vm_is_end()) {
    const auto instruction = vm_next_instruction();
    switch (instruction) {
    case OP_CONSTANT:
      const auto constant = vm_read_constant();
      stack_push(&vm.stack, constant);
      break;
    case OP_NIL:
      stack_push(&vm.stack, value_nil());
      break;
    case OP_TRUE:
      stack_push(&vm.stack, value_from_bool(true));
      break;
    case OP_FALSE:
      stack_push(&vm.stack, value_from_bool(false));
      break;
    case OP_ADD:
      if (!value_can_add(stack_peek_at(&vm.stack, 0),
                         stack_peek_at(&vm.stack, 1))) {
        vm_runtime_error("Operands must be numbers or strings.");
        return RUNTIME_ERROR;
      }
      vm_add();
      break;
    case OP_SUBTRACT:
      CHECK_BINARY_OPERANDS
      vm_subtract();
      break;
    case OP_MULTIPLY:
      CHECK_BINARY_OPERANDS
      vm_multiply();
      break;
    case OP_DIVIDE:
      CHECK_BINARY_OPERANDS
      vm_divide();
      break;
    case OP_NEGATE:
      CHECK_UNARY_OPERAND
      vm_negate();
      break;
    case OP_NOT:
      CHECK_NOT_OPERAND
      vm_negate();
      break;
    case OP_EQUAL:
      vm_equal();
      break;
    case OP_GREATER:
      CHECK_BINARY_OPERANDS
      vm_greater();
      break;
    case OP_LESS:
      CHECK_BINARY_OPERANDS
      vm_less();
      break;
    case OP_RETURN: {
      value_print(stack_pop(&vm.stack));
      printf("\n");
      return OK;
    } break;
    default:
      break;
    }
  }
  // FIXME: RUNTIME_ERROR?
  return OK;
}

bool vm_is_end() { return (vm.ip >= vm.chunk->count); }

byte_t vm_next_instruction() {
  if (vm.ip >= vm.chunk->count) {
    // FIXME: Error?
    return '\0';
  }
  auto const ip = vm.ip;
  auto const instruction = vm.chunk->code[ip];
#ifdef DEBUG_TRACE_EXECUTION
  debug_instruction(vm.chunk, ip);
#endif
  vm.ip++;
  return instruction;
}

value_t vm_read_constant() {
  const auto offset = (offset_t)vm_next_instruction();
  return vm.chunk->constants.values[offset];
}

value_t s(const value_t a, const value_t b) {
  return value_from_number(value_as_number(a) - value_as_number(b));
}

value_t m(const value_t a, const value_t b) {
  return value_from_number(value_as_number(a) * value_as_number(b));
}

value_t d(const value_t a, const value_t b) {
  return value_from_number(value_as_number(a) / value_as_number(b));
}

value_t n(const value_t a) {
  if (value_is_bool(a))
    return value_from_bool(!value_as_bool(a));
  if (value_is_nil(a))
    return value_from_bool(true);
  return value_from_number(-value_as_number(a));
}

value_t b(const value_t a, const value_t b) {
  if (a.type != b.type)
    return value_from_bool(false);
  switch (a.type) {
  case VAL_BOOL:
    return value_from_bool(value_as_bool(a) == value_as_bool(b));
  case VAL_NIL:
    return value_from_bool(true);
  case VAL_NUMBER:
    return value_from_bool(value_as_number(a) == value_as_number(b));
  default:
    break;
  }
  return value_from_bool(false);
}

void vm_add() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  auto const result = value_add(lhs, rhs);
  // FIXME: Replace when garbage collector is in place
  chunk_add_constant(vm.chunk, result);
  stack_push(&vm.stack, result);
}

void vm_subtract() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack, s(lhs, rhs));
}

void vm_multiply() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack, m(lhs, rhs));
}

void vm_divide() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack, d(lhs, rhs));
}

void vm_negate() { stack_push(&vm.stack, n(stack_pop(&vm.stack))); }

void vm_equal() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack, b(lhs, rhs));
}

void vm_greater() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack,
             value_from_bool(value_as_number(lhs) > value_as_number(rhs)));
}

void vm_less() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack,
             value_from_bool(value_as_number(lhs) < value_as_number(rhs)));
}

void vm_runtime_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);
  int line = vm.chunk->lines[vm.ip];
  fprintf(stderr, "[line %d] in script\n", line);
}

// TODO: mv to own source
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

value_t stack_peek_at(stack_t *stack, int at) {
  assert(!stack_is_empty(stack));
  auto const index = stack->top - 1 - at;
  return stack->values[index];
}
