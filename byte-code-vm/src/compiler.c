#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "string.h"
#include "value.h"

typedef enum {
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
  chunk_t *chunk;
  token_t previous, current;
  bool has_err;
} parser_t;

parser_t parser;

void parser_init(chunk_t *chunk) {
  parser.chunk = chunk;
  parser.previous = parser.current = (token_t){
      .type = TOKEN_EOF,
      .lexeme = _(""),
  };
  parser.has_err = false;
}

void compiler_error(const string_t message);
void compiler_error_at(const token_t token, const string_t message);
bool compiler_is_end();
void compiler_advance();
void compiler_emit_byte(byte_t code);
void compiler_emit_constant(value_t value);
void compiler_emit_return();
void compiler_consume(token_type type, const string_t message);
void compiler_end();
void compiler_expression();
void compiler_grouping();
void compiler_unary();
void compiler_binary();
void compiler_number();
void compiler_literal();
void compiler_string();
void compiler_parse_precedence(precedence_t precedence);
parse_rule_t compiler_get_rule(token_type type);

bool compiler_run(const string_t source, chunk_t *chunk) {
  parser_init(chunk);
  scanner_init(source);
  compiler_expression();
  compiler_consume(TOKEN_EOF, _("Expect end of compiler_expression."));
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

void compiler_advance() {
  auto current = parser.previous = parser.current;
  while ((current = scanner_next_token()).type != TOKEN_EOF) {
    if (current.type != TOKEN_ERROR) {
      parser.current = current;
      break;
    }
    compiler_error_at(current, _("Unexpected compiler_expression."));
  }
}

void compiler_emit_byte(byte_t code) {
  chunk_write(parser.chunk, code, parser.previous.line);
}

void compiler_emit_constant(value_t value) {
  auto offset = chunk_add_constant(parser.chunk, value);
  if (offset >= UINT8_MAX) {
    compiler_error(_("Too many constants in one chunk."));
    return;
  }
  compiler_emit_byte(OP_CONSTANT);
  compiler_emit_byte(offset);
}

void compiler_emit_return() { compiler_emit_byte(OP_RETURN); }

void compiler_consume(token_type type, const string_t message) {
  if (parser.current.type == type) {
    compiler_advance();
  } else {
    compiler_error_at(parser.current, message);
  }
}

void compiler_end() { return compiler_emit_return(); }

void compiler_expression() { compiler_parse_precedence(PREC_ASSIGNMENT); }

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
    compiler_emit_byte(OP_EQUAL);
    compiler_emit_byte(OP_NOT);
    break;
  case TOKEN_EQUAL_EQUAL:
    // TODO:
    compiler_emit_byte(OP_EQUAL);
    break;
  case TOKEN_GREATER:
    compiler_emit_byte(OP_GREATER);
    break;
  case TOKEN_GREATER_EQUAL:
    compiler_emit_byte(OP_LESS);
    compiler_emit_byte(OP_NOT);
    break;
  case TOKEN_LESS:
    compiler_emit_byte(OP_LESS);
    break;
  case TOKEN_LESS_EQUAL:
    compiler_emit_byte(OP_GREATER);
    compiler_emit_byte(OP_NOT);
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
  auto value = value_from_string(parser.previous.lexeme);
  compiler_emit_constant(value);
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
  prefixRule();
  while (precedence <= compiler_get_rule(parser.current.type).precedence) {
    compiler_advance();
    auto infixRule = compiler_get_rule(parser.previous.type).infix;
    infixRule();
  }
}
