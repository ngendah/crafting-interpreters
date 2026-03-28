#pragma once

#include <cctype>
#include <fmt/core.h>
#include <string>
#include <sys/types.h>

#include "common.hpp"

struct Scanner {
  explicit Scanner(const std::string source) : source(source) {}

  Tokens scan() {
    Tokens tokens = {};
    std::string::iterator start = source.begin(), end = source.end(),
                          it = source.begin();
    while (it != end) {
      if (is_operator(*it)) {
        tokens.push_back(
            $new<Operator>((operator_type)(*it), std::string(start, it + 1)));

      } else if (is_identifier(*it)) {
        while ((it + 1) != end && std::isalnum(*(it + 1))) {
          it++;
        }
        tokens.push_back(
            $new<Identifier>(IDENTIFIER, std::string(start, it + 1)));
      } else {
        if (!is_space(*it)) {
          fmt::print("Scanner error, unrecognized charater '{}' at {}.\n", *it,
                     1 + (int)(end - it));
        }
      }
      start = ++it;
    }
    return tokens;
  }

  bool is_operator(char chr) {
    switch (chr) {
    case EQUAL:
    case ADD:
    case MINUS:
    case MULTIPLY:
    case DIVIDE:
    case CARET:
      return true;
    }
    return false;
  }

  bool is_identifier(char chr) { return ::isalnum(chr); }

  bool is_space(char chr) { return ::isspace(chr) || ::isblank(chr); }

protected:
  std::string source;
};
