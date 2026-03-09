#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  const char *start, *eof;
  char *current;
  Line line;
} Scanner;

Scanner scanner;

__inline(bool) isAtEnd();
__inline(Token) makeToken(TokenType type);
__inline(Token) string();
__inline(Token) number();
__inline(Token) identifier();
__inline(Token) errorToken(const String errorMessage);
__inline(const char) advance();
__inline(const char) peek();
__inline(const char) peekNext();
__inline(bool) match(const char chr);
__inline_void skipWhitespace();

Scanner *const getScanner() { return &scanner; }

const TokenType checkKeyword(const int start, const String rest,
                             const TokenType type) {
  if ((size_t)(scanner.current - scanner.start) == start + rest.length &&
      memcmp(scanner.start + start, rest.str, rest.length) == 0) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}

void initScanner(const String source) {
  scanner.start = source.str;
  scanner.current = (char *)source.str;
  scanner.eof = (source.str + source.length);
  scanner.line = 1;
}

const Token scanToken() {
  skipWhitespace();
  scanner.start = scanner.current;
  if (isAtEnd())
    return makeToken(TOKEN_EOF);
  const char chr = advance();
  if (isalpha(chr))
    return identifier();
  if (isdigit(chr))
    return number();
  switch (chr) {
  case '(': {
    return makeToken(TOKEN_LEFT_PAREN);
  } break;
  case ')': {
    return makeToken(TOKEN_RIGHT_PAREN);
  } break;
  case '{': {
    return makeToken(TOKEN_LEFT_BRACE);
  } break;
  case '}': {
    return makeToken(TOKEN_RIGHT_BRACE);
  } break;
  case ';': {
    return makeToken(TOKEN_SEMICOLON);
  } break;
  case ',': {
    return makeToken(TOKEN_COMMA);
  } break;
  case '.': {
    return makeToken(TOKEN_DOT);
  } break;
  case '-': {
    return makeToken(TOKEN_MINUS);
  } break;
  case '+': {
    return makeToken(TOKEN_PLUS);
  } break;
  case '/': {
    if (peek() == '/') {
      while (peek() != '\n' && !isAtEnd())
        advance();
    } else {
      return makeToken(TOKEN_SLASH);
    }
  } break;
  case '*': {
    return makeToken(TOKEN_STAR);
  } break;
  case '!': {
    return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  } break;
  case '=': {
    return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  } break;
  case '<': {
    return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  } break;
  case '>': {
    return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  } break;
  case '\n': {
    scanner.line++;
    advance();
  } break;
  case '"': {
    return string();
  } break;
  default:
    break;
  }
  scanner.start = scanner.current;
  if (isAtEnd()) {
    return makeToken(TOKEN_EOF);
  }
  return errorToken(_("Unexpected character."));
}

__inline(bool) isAtEnd() { return scanner.current == scanner.eof; }

__inline(Token) makeToken(TokenType type) {
  const Token token = {
      .type = type,
      .lexeme =
          {
              .str = scanner.start,
              .length = (size_t)(scanner.current - scanner.start),
          },
      .line = scanner.line};
  return token;
}

__inline(Token) string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      scanner.line++;
    advance();
  }
  if (isAtEnd())
    return errorToken(_("Unterminated string"));
  advance();
  return makeToken(TOKEN_STRING);
}

__inline(Token) number() {
  while (isdigit(peek()))
    advance();
  if (peek() == '.' && isdigit(peekNext())) {
    advance();
    while (isdigit(peek()))
      advance();
  }
  return makeToken(TOKEN_NUMBER);
}

__inline(TokenType) identifierType() {
  switch (*scanner.start) {
  case 'a':
    return checkKeyword(1, _("nd"), TOKEN_AND);
  case 'c':
    return checkKeyword(1, _("lass"), TOKEN_CLASS);
  case 'e':
    return checkKeyword(1, _("lse"), TOKEN_ELSE);
  case 'f':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'a':
        return checkKeyword(2, _("lse"), TOKEN_FALSE);
      case 'o':
        return checkKeyword(2, _("r"), TOKEN_FOR);
      case 'u':
        return checkKeyword(2, _("n"), TOKEN_FUN);
      }
    }
    break;
  case 'i':
    return checkKeyword(1, _("f"), TOKEN_IF);
  case 'n':
    return checkKeyword(1, _("il"), TOKEN_NIL);
  case 'o':
    return checkKeyword(1, _("r"), TOKEN_OR);
  case 'p':
    return checkKeyword(1, _("rint"), TOKEN_PRINT);
  case 'r':
    return checkKeyword(1, _("eturn"), TOKEN_RETURN);
  case 's':
    return checkKeyword(1, _("uper"), TOKEN_SUPER);
  case 't':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'h':
        return checkKeyword(2, _("is"), TOKEN_THIS);
      case 'r':
        return checkKeyword(2, _("ue"), TOKEN_TRUE);
      }
    }
    break;
  case 'v':
    return checkKeyword(1, _("ar"), TOKEN_VAR);
  case 'w':
    return checkKeyword(1, _("hile"), TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

__inline(Token) identifier() {
  while (isalpha(peek()) || isdigit(peek()))
    advance();
  return makeToken(identifierType());
}

__inline(Token) errorToken(const String errorMessage) {
  const Token token = {
      .type = TOKEN_ERROR,
      .lexeme = errorMessage,
      .line = scanner.line,
  };
  return token;
}

__inline(const char) advance() {
  const char chr = *scanner.current++;
  return chr;
}

__inline(bool) match(const char chr) {
  if (isAtEnd())
    return false;
  if (*scanner.current != chr)
    return false;
  scanner.current++;
  return true;
}

__inline(bool) isWhiteSpace(const char chr) {
  return chr == ' ' || chr == '\r' || chr == '\t';
}

__inline_void skipWhitespace() {
  while (isWhiteSpace(peek()) && !isAtEnd())
    advance();
}

__inline(const char) peek() { return *scanner.current; }

__inline(const char) peekNext() {
  if (isAtEnd())
    return '\0';
  return scanner.current[1];
}
