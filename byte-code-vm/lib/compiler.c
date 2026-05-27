#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "environment.h"
#include "scanner.h"
#include "value.h"
#include "variables.h"

#define LO_BYTE(x) ((x >> 8) & 0xff)
#define HI_BYTE(x) (x & 0xff)

typedef enum : byte_t {
  PREC_NONE = 0x01, // Lowest
  PREC_ASSIGNMENT,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,   // [==, !=]
  PREC_COMPARISON, // [<, >, <=, >=]
  PREC_TERM,       // [+, -]
  PREC_FACTOR,     // [*, /]
  PREC_UNARY,      // [!, -]
  PREC_CALL,       // [., ()]
  PREC_PRIMARY,
} precedence_t;

typedef void (*compiler_function_t)(void);

typedef struct {
  compiler_function_t prefix, infix;
  precedence_t precedence;
} parse_rule_t;

typedef struct {
  bool can_assign;
} precedence_context_t;

typedef struct {
  chunk_t *chunk;
  environment_t *environment;
  uint8_t scope;
  token_t previous, current;
  precedence_context_t precedence_context;
  bool has_err;
  bool debug_print_codes;
} parser_t;

parser_t parser;

void parser_init(chunk_t *chunk, environment_t *environment) {
  parser.chunk = chunk;
  parser.environment = environment;
  parser.previous = parser.current = (token_t){
      .type = TOKEN_EOF,
      .lexeme = _(""),
  };
  parser.has_err = false;
  parser.precedence_context = (precedence_context_t){
      .can_assign = false,
  };
  environment_scope_add(parser.environment);
  parser.scope = 0;
  parser.debug_print_codes = true;
}

void compiler_error(const string_t message);
void compiler_error_at(const token_t token, const string_t message);
bool compiler_is_end();
void compiler_advance();
offset_t compiler_emit_byte(byte_t code);
offset_t compiler_emit_word(byte_t code1, byte_t code2);
offset_t compiler_emit_half_dword(byte_t code1, byte_t code2, byte_t code3);
offset_t compiler_emit_jump_word(byte_t code);
offset_t compiler_instruction_pointer();
void compiler_emit_constant(value_t value);
void compiler_emit_return();
void compiler_emit_loop(uint16_t loop_start_at);
bool compiler_check(token_type type);
bool compiler_match(token_type type);
void compiler_consume(token_type type, const string_t message);
void compiler_end();
void compiler_declaration();
void compiler_declaration_variable();
void compiler_statement();
void compiler_statement_if();
void compiler_statement_while();
void compiler_statement_for();
void compiler_statement_print();
void compiler_statement_expression();
void compiler_expression();
void compiler_scope_begin();
void compiler_block();
void compiler_scope_end();
void compiler_grouping();
void compiler_unary();
void compiler_binary();
void compiler_number();
void compiler_literal();
void compiler_string();
void compiler_variable();
void compiler_and();
void compiler_or();
void compiler_patch_jump(uint16_t at);
void compiler_parse_precedence(precedence_t precedence);

void compiler_print_codes(int steps_back);

parse_rule_t compiler_get_rule(token_type type);

bool compiler_run(const string_t source, chunk_t *chunk,
                  environment_t *environment) {
#ifdef DEBUG_ENABLED
  printf("--- compiler run ---\n");
#endif
  parser_init(chunk, environment);
  scanner_init(source);
  compiler_advance();
  while (!compiler_is_end()) {
    compiler_declaration();
  }
  return !parser.has_err;
}

void compiler_error(const string_t message) {
  parser.has_err = true;
  compiler_error_at(parser.previous, message);
}

void compiler_error_at(const token_t token, const string_t message) {
  fprintf(stderr, "[line %d] compiler_error", token.line);
  if (token.type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token.type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at %.*s", (int)token.lexeme.length, token.lexeme.str);
  }
  fprintf(stderr, ": %s\n", message.str);
}

bool compiler_is_end() { return parser.current.type == TOKEN_EOF; }

bool compiler_check(token_type type) { return parser.current.type == type; }

bool compiler_match(token_type type) {
  if (parser.current.type != type) {
    return false;
  }
  compiler_advance();
  return true;
}

void compiler_consume(token_type type, const string_t message) {
  if (parser.current.type == type) {
    compiler_advance();
  } else {
    compiler_error_at(parser.current, message);
  }
}

void compiler_advance() {
  auto current = parser.previous = parser.current;
  current = scanner_next_token();
  parser.current = current;
}

void compiler_end() { return compiler_emit_return(); }

offset_t compiler_emit_byte(byte_t code) {
  const auto offset = chunk_write(parser.chunk, code, parser.previous.line);
#ifdef DEBUG_ENABLED
  compiler_print_codes(1);
#endif
  return offset;
}

offset_t compiler_emit_word(byte_t code1, byte_t code2) {
  chunk_write(parser.chunk, code1, parser.previous.line);
  const auto offset = chunk_write(parser.chunk, code2, parser.previous.line);
#ifdef DEBUG_ENABLED
  compiler_print_codes(2);
#endif
  return offset;
}

offset_t compiler_emit_half_dword(byte_t code1, byte_t code2, byte_t code3) {
  chunk_write(parser.chunk, code1, parser.previous.line);
  chunk_write(parser.chunk, code2, parser.previous.line);
  const auto offset = chunk_write(parser.chunk, code3, parser.previous.line);
#ifdef DEBUG_ENABLED
  compiler_print_codes(3);
#endif
  return offset;
}

offset_t compiler_emit_jump_word(byte_t code) {
  compiler_emit_byte(code);
  return compiler_emit_word(0xff, 0xff);
}

offset_t compiler_instruction_pointer() {
  auto at = chunk_length(parser.chunk);
  return at == 0 ? at : at - 1;
}

void compiler_emit_constant(value_t value) {
  auto offset = chunk_add_constant(parser.chunk, value);
  if (offset >= UINT8_MAX) {
    compiler_error(_("Too many constants in one chunk."));
    return;
  }
  compiler_emit_word(OP_CONSTANT, offset);
}

void compiler_emit_return() { compiler_emit_byte(OP_RETURN); }

void compiler_emit_loop(uint16_t loop_start_at) {
  compiler_emit_byte(OP_LOOP);
  // NOTE: -1 to point to the beginning of the 2 byte jmp instruction
  // + 2 to point to the beginning of the loop instruction (skipping the jump
  // op_code)
  const auto loop_start_ip = loop_start_at - 1 + 2;
  const auto ip = compiler_instruction_pointer();
  const auto jmp_steps = ip - loop_start_ip;
  assert(jmp_steps < UINT16_MAX);
#ifdef DEBUG_ENABLED
  auto print_codes = parser.debug_print_codes;
  parser.debug_print_codes = false;
#endif
  compiler_emit_word(LO_BYTE(jmp_steps), HI_BYTE(jmp_steps));
#ifdef DEBUG_ENABLED
  printf("loop: ip=%d, jump_steps=%d, loop_start_at=%d, loop_back_ip=%d\n", ip,
         -jmp_steps, loop_start_ip, ip - jmp_steps);
  parser.debug_print_codes = print_codes;
#endif
}

void compiler_declaration() {
  if (compiler_check(TOKEN_VAR)) {
    compiler_advance();
    compiler_declaration_variable();
  } else if (compiler_check(TOKEN_LEFT_BRACE)) {
    compiler_block();
  } else {
    compiler_statement();
  }
  // FIXME: implement error handling
}

void compiler_declaration_variable() {
  compiler_consume(TOKEN_IDENTIFIER, _("Expect variable name."));
  if (variables_exist(parser.environment->scopes[parser.scope].variables,
                      parser.previous.lexeme)) {
    compiler_error(_("Variable already exists in the current scope."));
  }
  const auto offset =
      variables_add(parser.environment->scopes[parser.scope].variables,
                    parser.previous.lexeme, VAR_HIDDEN);
  compiler_emit_half_dword(OP_VARIABLE_DEFINE, parser.scope, offset);
  if (compiler_match(TOKEN_EQUAL)) {
    compiler_expression();
  } else {
    compiler_emit_byte(OP_NIL);
  }
  compiler_emit_half_dword(OP_VARIABLE_SET, parser.scope, offset);
  variables_variable_set_visible(
      parser.environment->scopes[parser.scope].variables, offset);
  compiler_consume(TOKEN_SEMICOLON,
                   _("Expect ';' after variable declaration."));
}

void compiler_statement() {
  if (compiler_match(TOKEN_PRINT)) {
    compiler_statement_print();
  } else if (compiler_match(TOKEN_IF)) {
    compiler_statement_if();
  } else if (compiler_match(TOKEN_WHILE)) {
    compiler_statement_while();
  } else if (compiler_match(TOKEN_FOR)) {
    compiler_statement_for();
  } else {
    compiler_statement_expression();
  }
}

void compiler_statement_print() {
  compiler_expression();
  compiler_consume(TOKEN_SEMICOLON, _("Expect ';' after value."));
  compiler_emit_byte(OP_PRINT);
}

void compiler_assert_jump(offset_t offset) {
  if (offset > UINT16_MAX) {
    compiler_error(_("Cannot make long jumps."));
    assert(offset <= UINT16_MAX);
  }
}

void compiler_statement_if() {
  compiler_consume(TOKEN_LEFT_PAREN, _("Expect '(' after 'if'."));
  compiler_expression();
  compiler_consume(TOKEN_RIGHT_PAREN, _("Expect ')' after condition."));
  // NOTE: Record the start of the if-block
  // set the number of instructions to jump to some dummy value
  auto jmp_code_at = compiler_emit_jump_word(OP_JUMP_IF_FALSE);
  compiler_emit_byte(OP_POP);
  compiler_block();
  // NOTE: Calculate the actual number of instructions to jump
  // replace the dummy jump values with the actual values.
  compiler_patch_jump(jmp_code_at);
  compiler_emit_byte(OP_POP);
  if (compiler_match(TOKEN_ELSE)) {
    compiler_block();
    jmp_code_at = compiler_emit_jump_word(OP_JUMP);
    compiler_patch_jump(jmp_code_at);
  }
}

void compiler_statement_while() {
  const auto loop_start_at = compiler_instruction_pointer();
  compiler_consume(TOKEN_LEFT_PAREN, _("Expect '(' after 'while'."));
  compiler_expression();
  compiler_consume(TOKEN_RIGHT_PAREN, _("Expect ')' after condition."));
  auto jmp_code_at = compiler_emit_jump_word(OP_JUMP_IF_FALSE);
  compiler_emit_byte(OP_POP);
  compiler_block();
  compiler_emit_loop(loop_start_at);
  compiler_patch_jump(jmp_code_at);
  compiler_emit_byte(OP_POP);
}

void compiler_statement_for() {
  compiler_scope_begin();
  compiler_consume(TOKEN_LEFT_PAREN, _("Expect '(' after 'for'."));
  if (compiler_match(TOKEN_SEMICOLON)) {
    // NOTE: No initializer
  } else if (compiler_match(TOKEN_VAR)) {
    compiler_declaration_variable();
  } else {
    compiler_statement_expression();
  }
  auto loop_start_at = compiler_instruction_pointer();
  uint16_t jmp_code_at = UINT16_MAX;
  if (!compiler_check(TOKEN_SEMICOLON)) {
    compiler_expression();
    compiler_consume(TOKEN_SEMICOLON, _("Expect ';' after loop condition."));
    jmp_code_at = compiler_emit_jump_word(OP_JUMP_IF_FALSE);
    compiler_emit_byte(OP_POP);
  } else {
    compiler_consume(TOKEN_SEMICOLON, _("Expect ';' after loop condition."));
  }
  if (!compiler_check(TOKEN_RIGHT_PAREN)) {
    const auto jmp_at = compiler_emit_jump_word(OP_JUMP);
    const auto increment_at = compiler_instruction_pointer();
    compiler_expression();
    compiler_consume(TOKEN_RIGHT_PAREN, _("Expect ')' after 'for' clause."));
    compiler_emit_byte(OP_POP);
    compiler_emit_loop(loop_start_at);
    loop_start_at = increment_at;
    compiler_patch_jump(jmp_at);
  } else {
    compiler_consume(TOKEN_RIGHT_PAREN, _("Expect ')' after 'for' clause."));
  }
  compiler_block();
  compiler_emit_loop(loop_start_at);
  if (jmp_code_at != UINT16_MAX) {
    compiler_patch_jump(jmp_code_at);
    compiler_emit_byte(OP_POP);
  }
  compiler_scope_end();
}

void compiler_statement_expression() {
  compiler_expression();
  compiler_consume(TOKEN_SEMICOLON, _("Expect ';' after value."));
  compiler_emit_byte(OP_POP);
}

void compiler_expression() { compiler_parse_precedence(PREC_ASSIGNMENT); }

void compiler_scope_begin() {
  environment_scope_add(parser.environment);
  parser.scope++;
}

void compiler_block() {
  compiler_consume(TOKEN_LEFT_BRACE, _("Expect '{' before block."));
  compiler_scope_begin();
  while (!compiler_check(TOKEN_RIGHT_BRACE) && !compiler_check(TOKEN_EOF)) {
    compiler_declaration();
  }
  compiler_scope_end();
  compiler_consume(TOKEN_RIGHT_BRACE, _("Expect '}' after block."));
}

void compiler_scope_end() {
  parser.scope--;
  compiler_emit_byte(OP_POP);
}

void compiler_grouping() {
  compiler_expression();
  compiler_consume(TOKEN_RIGHT_PAREN,
                   _("Expect ')' after compiler_expression."));
}

void compiler_unary() {
  auto operatorType = parser.previous.type;
  compiler_expression();
  switch (operatorType) {
  case TOKEN_MINUS:
    compiler_emit_byte(OP_NEGATE);
    break;
  case TOKEN_BANG:
    compiler_emit_byte(OP_NOT);
    break;
  default:
    break;
  }
  return;
}

void compiler_binary() {
  auto operatorType = parser.previous.type;
  auto rule = compiler_get_rule(operatorType);
  compiler_parse_precedence(rule.precedence);
  switch (operatorType) {
  case TOKEN_PLUS:
    compiler_emit_byte(OP_ADD);
    break;
  case TOKEN_MINUS:
    compiler_emit_byte(OP_SUBTRACT);
    break;
  case TOKEN_STAR:
    compiler_emit_byte(OP_MULTIPLY);
    break;
  case TOKEN_SLASH:
    compiler_emit_byte(OP_DIVIDE);
    break;
  case TOKEN_BANG_EQUAL:
    compiler_emit_word(OP_EQUAL, OP_NOT);
    break;
  case TOKEN_EQUAL_EQUAL:
    // TODO:
    compiler_emit_byte(OP_EQUAL);
    break;
  case TOKEN_GREATER:
    compiler_emit_byte(OP_GREATER);
    break;
  case TOKEN_GREATER_EQUAL:
    compiler_emit_word(OP_LESS, OP_NOT);
    break;
  case TOKEN_LESS:
    compiler_emit_byte(OP_LESS);
    break;
  case TOKEN_LESS_EQUAL:
    compiler_emit_word(OP_GREATER, OP_NOT);
    break;
  default:
    break;
  }
}

void compiler_number() {
  auto value = string_as_double(parser.previous.lexeme);
  compiler_emit_constant(value_from_number(value));
}

void compiler_literal() {
  switch (parser.previous.type) {
  case TOKEN_FALSE:
    compiler_emit_byte(OP_FALSE);
    break;
  case TOKEN_NIL:
    compiler_emit_byte(OP_NIL);
    break;
  case TOKEN_TRUE:
    compiler_emit_byte(OP_TRUE);
    break;
  default:
    break;
  }
}

void compiler_string() {
  auto value =
      value_from_string(parser.previous.lexeme,
                        parser.environment->scopes[parser.scope].hash_table);
  compiler_emit_constant(value);
}

void compiler_and() {
  // NOTE: Chapter 23.2
  auto jmp_code_at = compiler_emit_jump_word(OP_JUMP_IF_FALSE);
  compiler_emit_byte(OP_POP);
  compiler_parse_precedence(PREC_AND);
  compiler_patch_jump(jmp_code_at);
}

void compiler_or() {
  // NOTE: Chapter 23.2.1
  auto else_jmp_code_at = compiler_emit_jump_word(OP_JUMP_IF_FALSE);
  auto end_jmp_code_at = compiler_emit_jump_word(OP_JUMP);
  compiler_patch_jump(else_jmp_code_at);
  compiler_emit_byte(OP_POP);
  compiler_parse_precedence(PREC_OR);
  compiler_patch_jump(end_jmp_code_at);
}

typedef struct {
  offset_t offset;
  int scope;
} variable_location_t;

variable_location_t compiler_variable_search(int startAt,
                                             environment_t *environment,
                                             const string_t lexeme) {
  auto it = startAt;
  do {
    auto scope = environment->scopes[it];
    auto offset = variables_get_offset(scope.variables, lexeme, VAR_VISIBLE);
    if (offset > -1) {
      return (variable_location_t){.scope = it, .offset = offset};
    }
    it--;
  } while (it > -1);
  return (variable_location_t){.scope = -1, .offset = 0};
}

void compiler_variable() {
  const auto variable_location = compiler_variable_search(
      parser.scope, parser.environment, parser.previous.lexeme);
  assert(variable_location.scope > -1);
  if (compiler_match(TOKEN_EQUAL)) {
    compiler_expression();
    compiler_emit_half_dword(OP_VARIABLE_SET, variable_location.scope,
                             variable_location.offset);
    variables_variable_set_visible(
        parser.environment->scopes[variable_location.scope].variables,
        variable_location.offset);
  } else {
    compiler_emit_half_dword(OP_VARIABLE_GET, variable_location.scope,
                             variable_location.offset);
  }
}

parse_rule_t compiler_get_rule(token_type type) {

#define RULE(p, i, prec)                                                       \
  ((parse_rule_t){.prefix = (compiler_function_t)p,                            \
                  .infix = (compiler_function_t)i,                             \
                  .precedence = prec})

  switch (type) {
  case TOKEN_LEFT_PAREN:
    return RULE(compiler_grouping, nullptr, PREC_NONE);
  case TOKEN_MINUS:
    return RULE(compiler_unary, compiler_binary, PREC_TERM);
  case TOKEN_PLUS:
    return RULE(nullptr, compiler_binary, PREC_TERM);
  case TOKEN_SLASH:
    return RULE(nullptr, compiler_binary, PREC_FACTOR);
  case TOKEN_STAR:
    return RULE(nullptr, compiler_binary, PREC_FACTOR);
  case TOKEN_NUMBER:
    return RULE(compiler_number, nullptr, PREC_NONE);
  case TOKEN_FALSE:
    return RULE(compiler_literal, nullptr, PREC_NONE);
  case TOKEN_TRUE:
    return RULE(compiler_literal, nullptr, PREC_NONE);
  case TOKEN_NIL:
    return RULE(compiler_literal, nullptr, PREC_NONE);
  case TOKEN_BANG:
    return RULE(compiler_unary, nullptr, PREC_NONE);
  case TOKEN_BANG_EQUAL:
    return RULE(nullptr, compiler_binary, PREC_EQUALITY);
  case TOKEN_EQUAL_EQUAL:
    return RULE(nullptr, compiler_binary, PREC_EQUALITY);
  case TOKEN_GREATER:
    return RULE(nullptr, compiler_binary, PREC_COMPARISON);
  case TOKEN_GREATER_EQUAL:
    return RULE(nullptr, compiler_binary, PREC_COMPARISON);
  case TOKEN_LESS:
    return RULE(nullptr, compiler_binary, PREC_COMPARISON);
  case TOKEN_LESS_EQUAL:
    return RULE(nullptr, compiler_binary, PREC_COMPARISON);
  case TOKEN_STRING:
    return RULE(compiler_string, nullptr, PREC_NONE);
  case TOKEN_IDENTIFIER:
    return RULE(compiler_variable, nullptr, PREC_NONE);
  case TOKEN_AND:
    return RULE(nullptr, compiler_and, PREC_AND);
    break;
  case TOKEN_OR:
    return RULE(nullptr, compiler_or, PREC_OR);
    break;
  default:
    break;
  }
  return RULE(nullptr, nullptr, PREC_NONE);
}

void compiler_patch_jump(uint16_t at) {
  auto ip = compiler_instruction_pointer();
  auto patch_ip = at - 1;
  auto jmp_steps = ip - at;
  assert(jmp_steps < UINT16_MAX);
#ifdef DEBUG_ENABLED
  // NOTE: +2 because jump offsets are 2 bytes
  const auto from = patch_ip + 2;
  const auto to = from + jmp_steps;
  printf("jump_patch: ip=%d, patch_ip=%d, jump_steps=%d(%#X), from_ip=%d, "
         "to_ip=%d\n",
         ip, patch_ip, jmp_steps, jmp_steps, from, to);
#endif
  chunk_code_set_at(parser.chunk, patch_ip, LO_BYTE(jmp_steps));
  chunk_code_set_at(parser.chunk, patch_ip + 1, HI_BYTE(jmp_steps));
}

void compiler_parse_precedence(precedence_t precedence) {
  compiler_advance();
  auto prefixRule = compiler_get_rule(parser.previous.type).prefix;
  if (prefixRule == nullptr) {
    return compiler_error(_("Expect compiler_expression."));
  }
  parser.precedence_context = (precedence_context_t){
      .can_assign = precedence <= PREC_ASSIGNMENT,
  };
  prefixRule();
  while (precedence <= compiler_get_rule(parser.current.type).precedence) {
    compiler_advance();
    auto infixRule = compiler_get_rule(parser.previous.type).infix;
    infixRule();
    if (parser.precedence_context.can_assign && compiler_match(TOKEN_EQUAL)) {
      compiler_error(_("Invalid assignment target."));
    }
  }
  parser.precedence_context = (precedence_context_t){
      .can_assign = false,
  };
}

void compiler_print_codes(int steps_back) {
  if (!parser.debug_print_codes)
    return;
  const auto count = parser.chunk->count;
  const auto ip = count - steps_back;
  const auto ptr = parser.chunk->code;
  const auto line = parser.chunk->lines[ip];
  switch (abs(steps_back)) {
  case 3: {
    printf("line=%d: count=%zu, ip=%zu, opcode=code[%zu]=%s, "
           "scope=code[%zu]=0x%x, "
           "offset=code[%zu]=0x%x\n",
           line, count, ip, ip + 0, opcode_as_string[ptr[ip + 0]], ip + 1,
           ptr[ip + 1], ip + 2, ptr[ip + 2]);
  } break;
  case 2: {
    if (ptr[ip + 0] == 0xff) {
      printf("line=%d: count=%zu: ip=%zu, opcode=code[%zu]=0x%x, "
             "offset=code[%zu]=0x%x\n",
             line, count, ip, ip + 0, ptr[ip + 0], ip + 1, ptr[ip + 1]);
    } else {
      printf("line=%d: count=%zu: ip=%zu, opcode=code[%zu]=%s, "
             "offset=code[%zu]=0x%x\n",
             line, count, ip, ip + 0, opcode_as_string[ptr[ip + 0]], ip + 1,
             ptr[ip + 1]);
    }
  } break;
  case 1: {
    printf("line=%d: count=%zu: ip=%zu, opcode=code[%zu]=%s\n", line, count, ip,
           ip, opcode_as_string[ptr[ip + 0]]);
  } break;
  }
}
