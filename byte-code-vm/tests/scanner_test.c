#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

void test_comment() {
  scanner_init(_("// this is a comment"));
  auto const result = scanner_next_token();
  assert(result.type == TOKEN_EOF);
}

void test_number() {
  const string_t source = _("1.4");
  scanner_init(source);
  auto const result = scanner_next_token();
  assert(result.type == TOKEN_NUMBER);
  assert(string_compare(result.lexeme, source) == 0);
}

void test_tokens() {
  const string_t source = _("(){};,.-+*/=!><");
  scanner_init(source);
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_LEFT_PAREN);
    assert(string_compare(result.lexeme, _("(")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_RIGHT_PAREN);
    assert(string_compare(result.lexeme, _(")")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_LEFT_BRACE);
    assert(string_compare(result.lexeme, _("{")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_RIGHT_BRACE);
    assert(string_compare(result.lexeme, _("}")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_SEMICOLON);
    assert(string_compare(result.lexeme, _(";")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_COMMA);
    assert(string_compare(result.lexeme, _(",")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_DOT);
    assert(string_compare(result.lexeme, _(".")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_MINUS);
    assert(string_compare(result.lexeme, _("-")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_PLUS);
    assert(string_compare(result.lexeme, _("+")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_STAR);
    assert(string_compare(result.lexeme, _("*")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_SLASH);
    assert(string_compare(result.lexeme, _("/")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_EQUAL);
    assert(string_compare(result.lexeme, _("=")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_BANG);
    assert(string_compare(result.lexeme, _("!")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_GREATER);
    assert(string_compare(result.lexeme, _(">")) == 0);
  }
  {
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_LESS);
    assert(string_compare(result.lexeme, _("<")) == 0);
  }
}

void test_keywords() {
  {
    const auto source = _("if");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_IF);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("and");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_AND);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("class");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_CLASS);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("else");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_ELSE);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("and");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_AND);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("false");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_FALSE);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("for");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_FOR);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("fun");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_FUN);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("this");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_THIS);
    assert(string_compare(result.lexeme, source) == 0);
  }
  {
    const auto source = _("true");
    scanner_init(source);
    auto const result = scanner_next_token();
    assert(result.type == TOKEN_TRUE);
    assert(string_compare(result.lexeme, source) == 0);
  }
}

int main() {
  test_comment();
  test_tokens();
  test_number();
  test_keywords();
  return EXIT_SUCCESS;
}
