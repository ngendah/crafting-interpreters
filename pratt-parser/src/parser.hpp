#pragma once

#include <memory>

#include "common.hpp"
#include "fmt/core.h"

struct Expression {
  typedef std::shared_ptr<Expression> ptr;
  virtual const std::string to_string() const = 0;
};

struct Literal final : Expression {
  explicit Literal(const Token::ptr &token) : token(token) {}

  const std::string to_string() const override { return token->lexeme; }

protected:
  const Token::ptr token;
};

struct Prefix final : Expression {
  explicit Prefix(const Token::ptr &op, const Expression::ptr &right)
      : op(op), right(right) {}

  const std::string to_string() const override {
    return fmt::format("{}{}", op->lexeme, right->to_string());
  }

protected:
  const Token::ptr op;
  const Expression::ptr right;
};

struct Binary final : Expression {
  explicit Binary(const Expression::ptr &left, const Token::ptr &op,
                  const Expression::ptr &right)
      : left(left), op(op), right(right) {}

  const std::string to_string() const {
    return fmt::format("({}{}{})", left->to_string(), op->lexeme,
                       right->to_string());
  }

protected:
  const Expression::ptr left, right;
  const Token::ptr op;
};

template <class T> Expression::ptr $new(const Token::ptr &token) {
  return Expression::ptr(new T(token));
}

template <class T>
Expression::ptr $new(const Token::ptr &op, const Expression::ptr &right) {
  return Expression::ptr(new T(op, right));
}

template <class T>
Expression::ptr $new(const Expression::ptr &left, const Token::ptr &op,
                     const Expression::ptr &right) {
  return Expression::ptr(new T(left, op, right));
}

struct Parser {
  explicit Parser(const Tokens &tokens) : tokens(tokens) {
    it = this->tokens.begin();
  }

  Expression::ptr parse() { return _parse(0); }

protected:
  const Expression::ptr _parse(int precedence) {
    auto left = prefix(*it);
    while (!is_end() && (*it)->precedence() > precedence) {
      auto op = advance();
      left = infix(left, op);
    }
    return left;
  }

  const Expression::ptr prefix(const Token::ptr &token) {
    switch (token->type) {
    case IDENTIFIER: {
      auto literal = $new<Literal>(token);
      advance();
      return literal;
    } break;
    case OPERATOR:
      advance();
      auto operand = _parse(token->precedence());
      return $new<Prefix>(token, operand);
    }
    return nullptr;
  }

  const Expression::ptr infix(const Expression::ptr &left,
                              const Token::ptr &op) {
    if (op->type != OPERATOR)
      return nullptr; // Error;
    auto right = _parse(op->precedence());
    return $new<Binary>(left, op, right);
  }

  Token::ptr advance() {
    if (it == tokens.end())
      return nullptr;
    return *(it++);
  }

  bool is_end() { return it == tokens.end(); }

protected:
  Tokens tokens;
  Tokens::iterator it;
};
