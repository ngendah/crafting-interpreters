#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "environment.h"
#include "memory.h"
#include "stack.h"
#include "value.h"
#include "variables.h"
#include "vm.h"

bool vm_is_end();
vm_result_t vm_execute();
byte_t vm_instruction_next();
void vm_instruction_skip(int steps);
value_t vm_read_constant();
void vm_add();
void vm_subtract();
void vm_multiply();
void vm_divide();
void vm_negate();
void vm_equal();
void vm_greater();
void vm_less();
void vm_print();
void vm_jump();
void vm_jump_if_false();
void vm_variable_set();
void vm_variable_get();
void vm_runtime_error(const char *format, ...);

vm_t vm;

void vm_init() {
  vm.chunk = (chunk_t *)new(sizeof(chunk_t));
  vm.ip = 0;
  chunk_init(vm.chunk);
  stack_init(&vm.stack);
  environment_init(&vm.environment);
}

void vm_free() {
  chunk_free(vm.chunk);
  delete(vm.chunk);
  stack_free(&vm.stack);
  environment_free(&vm.environment);
  vm.chunk = nullptr;
  vm.ip = 0;
}

vm_result_t vm_run(const string_t source) {
  if (!compiler_run(source, vm.chunk, &vm.environment)) {
    return COMPILE_ERROR;
  }
  return vm_execute();
}

bool value_can_add(const value_t a, const value_t b) {
  return ((value_is_number(a) && value_is_number(b)) ||
          (value_is_string(a) && value_is_string(b)));
}

vm_result_t vm_execute() {

  printf("--- vm execute ---\n");

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
    const auto instruction = vm_instruction_next();
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
    case OP_VARIABLE_DEFINE:
      // NOTE: skip scope and offset
      vm_instruction_skip(2);
      break;
    case OP_VARIABLE_SET:
      vm_variable_set();
      break;
    case OP_VARIABLE_GET:
      vm_variable_get();
      break;
    case OP_JUMP:
      vm_jump();
      break;
    case OP_JUMP_IF_FALSE:
      vm_jump_if_false();
      break;
    case OP_RETURN: {
      return OK;
    } break;
    case OP_PRINT: {
      vm_print();
    } break;
    case OP_POP: {
      stack_pop(&vm.stack);
    } break;
    default:
      break;
    }
  }
  // FIXME: RUNTIME_ERROR?
  return OK;
}

bool vm_is_end() { return (vm.ip >= vm.chunk->count); }

byte_t vm_instruction_next() {
  assert(vm.ip < vm.chunk->count);
  auto const ip = vm.ip;
  auto const instruction = vm.chunk->code[ip];
#ifdef DEBUG_ENABLED
  printf("ip=%d, code[%d]=0x%x, opcode=%s\n", ip, ip, instruction,
         opcode_as_string[instruction]);
#endif
  vm.ip++;
  return instruction;
}

void vm_instruction_skip(int steps) {
  assert(vm.ip + steps < vm.chunk->count);
  vm.ip = vm.ip + steps;
}

byte_t vm_instruction_next_get() {
  assert(vm.ip < vm.chunk->count);
  auto const ip = vm.ip;
  auto const instruction = vm.chunk->code[ip];
  vm.ip++;
  return instruction;
}

value_t vm_read_constant() {
  const auto offset = (offset_t)vm_instruction_next_get();
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
  return value_from_bool(value_compare(a, b));
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

void vm_print() {
  const auto value = stack_pop(&vm.stack);
  value_print(value);
  printf("\n");
  stack_push(&vm.stack, value);
}

void vm_less() {
  auto const rhs = stack_pop(&vm.stack);
  auto const lhs = stack_pop(&vm.stack);
  stack_push(&vm.stack,
             value_from_bool(value_as_number(lhs) < value_as_number(rhs)));
}

#define MAKE_WORD(hi, lo) (uint16_t)((hi << 8 | lo))

uint16_t vm_read_word() {
  const auto hi_byte = vm_instruction_next_get();
  const auto lo_byte = vm_instruction_next_get();
  uint16_t word = MAKE_WORD(hi_byte, lo_byte);
  return word;
}

void vm_jump() {
  auto jump_steps = vm_read_word();
#ifdef DEBUG_ENABLED
  printf("ip=%d,jump_offset=%d\n", vm.ip, jump_steps);
#endif
  vm.ip += jump_steps;
}

void vm_jump_if_false() {
  auto condition = stack_pop(&vm.stack);
  auto jump_steps = vm_read_word();
#ifdef DEBUG_ENABLED
  printf("ip=%d, condition=%b, jump_offset=%d\n", vm.ip,
         value_is_falsey(condition), jump_steps);
#endif
  vm.ip += value_is_falsey(condition) * jump_steps;
  stack_push(&vm.stack, condition);
}

void vm_variable_set() {
  const auto scope = vm_instruction_next_get();
  const auto offset = vm_instruction_next_get();
  const auto value = stack_pop(&vm.stack);
  const auto variable =
      variables_get_at(vm.environment.scopes[scope].variables, offset);
  variable->value = value;
  stack_push(&vm.stack, value);
}

void vm_variable_get() {
  const auto scope = vm_instruction_next_get();
  const auto offset = vm_instruction_next_get();
  const auto variable =
      variables_get_at(vm.environment.scopes[scope].variables, offset);
  stack_push(&vm.stack, variable->value);
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
