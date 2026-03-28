#pragma once

#include <list>
#include <memory>
#include <string>
#include <sys/types.h>

typedef enum : u_int8_t {
  OPERATOR,
  IDENTIFIER,
} token_type;

typedef enum : u_int8_t {
  ADD = '+',
  MINUS = '-',
  MULTIPLY = '*',
  DIVIDE = '/',
  CARET = '^',
  EQUAL = '='
} operator_type;

struct Token {
  typedef std::shared_ptr<Token> ptr;

  explicit Token(const token_type type, const std::string lexeme)
      : type(type), lexeme(lexeme) {}

  virtual const int precedence() const = 0;

  const token_type type;
  const std::string lexeme;
};

typedef std::list<Token::ptr> Tokens;

struct Identifier : Token {
  explicit Identifier(const token_type type, const std::string lexeme)
      : Token(type, lexeme) {}

  const int precedence() const override { return 0; };
};

template <class T>
Token::ptr $new(const token_type type, const std::string lexeme) {
  return Token::ptr(new T(type, lexeme));
}

struct Operator : Token {
  explicit Operator(const operator_type op, const std::string lexeme)
      : op(op), Token(OPERATOR, lexeme) {}

  const int precedence() const override {
    switch (op) {
    case EQUAL:
    case ADD:
    case MINUS:
      return 1;
    case MULTIPLY:
    case DIVIDE:
      return 2;
    case CARET:
      return 3;
    }
    return 0;
  }

  const operator_type op;
};

template <class T>
Token::ptr $new(const operator_type op, const std::string lexeme) {
  return Token::ptr(new T(op, lexeme));
}
