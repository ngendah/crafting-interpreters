#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

void test_comment() {
  initScanner(_("// this is a comment"));
  auto const result = scanToken();
  assert(result.type == TOKEN_EOF);
}

void test_number() {
  const String source = _("1.4");
  initScanner(source);
  auto const result = scanToken();
  assert(result.type == TOKEN_NUMBER);
  assert(cmp(result.lexeme, source) == 0);
}

void test_tokens() {
  const String source = _("(){};,.-+*/=!><");
  initScanner(source);
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_LEFT_PAREN);
    assert(cmp(result.lexeme, _("(")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_RIGHT_PAREN);
    assert(cmp(result.lexeme, _(")")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_LEFT_BRACE);
    assert(cmp(result.lexeme, _("{")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_RIGHT_BRACE);
    assert(cmp(result.lexeme, _("}")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_SEMICOLON);
    assert(cmp(result.lexeme, _(";")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_COMMA);
    assert(cmp(result.lexeme, _(",")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_DOT);
    assert(cmp(result.lexeme, _(".")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_MINUS);
    assert(cmp(result.lexeme, _("-")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_PLUS);
    assert(cmp(result.lexeme, _("+")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_STAR);
    assert(cmp(result.lexeme, _("*")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_SLASH);
    assert(cmp(result.lexeme, _("/")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_EQUAL);
    assert(cmp(result.lexeme, _("=")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_BANG);
    assert(cmp(result.lexeme, _("!")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_GREATER);
    assert(cmp(result.lexeme, _(">")) == 0);
  }
  {
    auto const result = scanToken();
    assert(result.type == TOKEN_LESS);
    assert(cmp(result.lexeme, _("<")) == 0);
  }
}

void test_keywords() {
  {
    const auto source = _("if");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_IF);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("and");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_AND);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("class");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_CLASS);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("else");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_ELSE);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("and");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_AND);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("false");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_FALSE);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("for");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_FOR);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("fun");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_FUN);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("this");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_THIS);
    assert(cmp(result.lexeme, source) == 0);
  }
  {
    const auto source = _("true");
    initScanner(source);
    auto const result = scanToken();
    assert(result.type == TOKEN_TRUE);
    assert(cmp(result.lexeme, source) == 0);
  }
}

int main() {
  test_comment();
  test_tokens();
  test_number();
  test_keywords();
  return EXIT_SUCCESS;
}
