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
}

void compiler_error(const string_t message);
void compiler_error_at(const token_t token, const string_t message);
bool compiler_is_end();
void compiler_advance();
void compiler_emit_byte(byte_t code);
void compiler_emit_word(byte_t code1, byte_t code2);
void compiler_emit_constant(value_t value);
void compiler_emit_return();
bool compiler_check(token_type type);
bool compiler_match(token_type type);
void compiler_consume(token_type type, const string_t message);
void compiler_end();
void compiler_declaration();
void compiler_declaration_variable();
void compiler_statement();
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
void compiler_parse_precedence(precedence_t precedence);

void compiler_print_byte(byte_t code);
void compiler_print_word(byte_t code1, byte_t code2);
void compiler_print_half_dword(byte_t code1, byte_t code2, byte_t code3);
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

void compiler_emit_byte(byte_t code) {
  chunk_write(parser.chunk, code, parser.previous.line);
#ifdef DEBUG_ENABLED
  compiler_print_byte(code);
  compiler_print_codes(1);
#endif
}

void compiler_emit_word(byte_t code1, byte_t code2) {
  chunk_write(parser.chunk, code1, parser.previous.line);
  chunk_write(parser.chunk, code2, parser.previous.line);
#ifdef DEBUG_ENABLED
  compiler_print_word(code1, code2);
  compiler_print_codes(2);
#endif
}

void compiler_emit_half_dword(byte_t code1, byte_t code2, byte_t code3) {
  chunk_write(parser.chunk, code1, parser.previous.line);
  chunk_write(parser.chunk, code2, parser.previous.line);
  chunk_write(parser.chunk, code3, parser.previous.line);
#ifdef DEBUG_ENABLED
  compiler_print_half_dword(code1, code2, code3);
  compiler_print_codes(3);
#endif
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

void compiler_declaration() {
  if (compiler_match(TOKEN_VAR)) {
    compiler_declaration_variable();
  } else if (compiler_match(TOKEN_LEFT_BRACE)) {
    compiler_scope_begin();
    compiler_block();
    compiler_scope_end();
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
  } else {
    compiler_statement_expression();
  }
}

void compiler_statement_print() {
  compiler_expression();
  compiler_consume(TOKEN_SEMICOLON, _("Expect ';' after value."));
  compiler_emit_byte(OP_PRINT);
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
  while (!compiler_check(TOKEN_RIGHT_BRACE) && !compiler_check(TOKEN_EOF)) {
    compiler_declaration();
  }
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
  default:
    break;
  }
  return RULE(nullptr, nullptr, PREC_NONE);
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

void compiler_print_byte(byte_t code) {
  //  printf("%s: code=%s\n", __PRETTY_FUNCTION__, opcode_as_string[code]);
}

void compiler_print_word(byte_t code, byte_t arg) {
  //  printf("%s: code=%s, offset=%d\n", __PRETTY_FUNCTION__,
  //        opcode_as_string[code], arg);
}

void compiler_print_half_dword(byte_t code, byte_t arg1, byte_t arg2) {
  // printf("%s: code=%s, scope=%d, offset=%d\n", __PRETTY_FUNCTION__,
  //        opcode_as_string[code], arg1, arg2);
}

void compiler_print_codes(int steps_back) {
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
    printf("line=%d: count=%zu: ip=%zu, opcode=code[%zu]=%s, "
           "offset=code[%zu]=0x%x\n",
           line, count, ip, ip + 0, opcode_as_string[ptr[ip + 0]], ip + 1,
           ptr[ip + 1]);
  } break;
  case 1: {
    printf("line=%d: count=%zu: ip=%zu, opcode=code[%zu]=%s\n", line, count, ip,
           ip, opcode_as_string[ptr[ip + 0]]);
  } break;
  }
}
