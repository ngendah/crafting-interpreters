#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"

typedef struct {
  const char *start, *eof;
  char *current;
  line_t line;
} scanner_t;

scanner_t scanner;

bool scanner_is_end();
token_t scanner_make_token(token_type type);
token_t scanner_string();
token_t scanner_number();
token_t scanner_identifier();
token_t scanner_error_token(const string_t message);
const char scanner_advance();
const char scanner_peek();
const char scanner_peek_next();
bool scanner_match(const char chr);
void scanner_skip_whitespace();

const token_type scanner_check_keyword(size_t start, const string_t rest,
                                       const token_type type) {
  if ((size_t)(scanner.current - scanner.start) == start + rest.length &&
      memcmp(scanner.start + start, rest.str, rest.length) == 0) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}

void scanner_init(const string_t source) {
  scanner.start = source.str;
  scanner.current = (char *)source.str;
  scanner.eof = (source.str + source.length);
  scanner.line = 1;
}

const token_t scanner_next_token() {
  scanner_skip_whitespace();
  scanner.start = scanner.current;
  if (scanner_is_end())
    return scanner_make_token(TOKEN_EOF);
  const char chr = scanner_advance();
  if (isalpha(chr))
    return scanner_identifier();
  if (isdigit(chr))
    return scanner_number();
  switch (chr) {
  case '(': {
    return scanner_make_token(TOKEN_LEFT_PAREN);
  } break;
  case ')': {
    return scanner_make_token(TOKEN_RIGHT_PAREN);
  } break;
  case '{': {
    return scanner_make_token(TOKEN_LEFT_BRACE);
  } break;
  case '}': {
    return scanner_make_token(TOKEN_RIGHT_BRACE);
  } break;
  case ';': {
    return scanner_make_token(TOKEN_SEMICOLON);
  } break;
  case ',': {
    return scanner_make_token(TOKEN_COMMA);
  } break;
  case '.': {
    return scanner_make_token(TOKEN_DOT);
  } break;
  case '-': {
    return scanner_make_token(TOKEN_MINUS);
  } break;
  case '+': {
    return scanner_make_token(TOKEN_PLUS);
  } break;
  case '/': {
    if (scanner_peek() == '/') {
      while (scanner_peek() != '\n' && !scanner_is_end())
        scanner_advance();
    } else {
      return scanner_make_token(TOKEN_SLASH);
    }
  } break;
  case '*': {
    return scanner_make_token(TOKEN_STAR);
  } break;
  case '!': {
    return scanner_make_token(scanner_match('=') ? TOKEN_BANG_EQUAL
                                                 : TOKEN_BANG);
  } break;
  case '=': {
    return scanner_make_token(scanner_match('=') ? TOKEN_EQUAL_EQUAL
                                                 : TOKEN_EQUAL);
  } break;
  case '<': {
    return scanner_make_token(scanner_match('=') ? TOKEN_LESS_EQUAL
                                                 : TOKEN_LESS);
  } break;
  case '>': {
    return scanner_make_token(scanner_match('=') ? TOKEN_GREATER_EQUAL
                                                 : TOKEN_GREATER);
  } break;
  case '\n': {
    scanner.line++;
    scanner_advance();
  } break;
  case '"': {
    return scanner_string();
  } break;
  default:
    break;
  }
  scanner.start = scanner.current;
  if (scanner_is_end()) {
    return scanner_make_token(TOKEN_EOF);
  }
  return scanner_error_token(_("Unexpected character."));
}

bool scanner_is_end() { return scanner.current == scanner.eof; }

token_t scanner_make_token(token_type type) {
  const token_t token = {
      .type = type,
      .lexeme =
          {
              .str = scanner.start,
              .length = (size_t)(scanner.current - scanner.start),
          },
      .line = scanner.line};
  return token;
}

token_t scanner_string() {
  while (scanner_peek() != '"' && !scanner_is_end()) {
    if (scanner_peek() == '\n')
      scanner.line++;
    scanner_advance();
  }
  if (scanner_is_end())
    return scanner_error_token(_("Unterminated string"));
  scanner_advance();
  return scanner_make_token(TOKEN_STRING);
}

token_t scanner_number() {
  while (isdigit(scanner_peek()))
    scanner_advance();
  if (scanner_peek() == '.' && isdigit(scanner_peek_next())) {
    scanner_advance();
    while (isdigit(scanner_peek()))
      scanner_advance();
  }
  return scanner_make_token(TOKEN_NUMBER);
}

token_type scanner_identifier_type() {
  switch (*scanner.start) {
  case 'a':
    return scanner_check_keyword(1, _("nd"), TOKEN_AND);
  case 'c':
    return scanner_check_keyword(1, _("lass"), TOKEN_CLASS);
  case 'e':
    return scanner_check_keyword(1, _("lse"), TOKEN_ELSE);
  case 'f':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'a':
        return scanner_check_keyword(2, _("lse"), TOKEN_FALSE);
      case 'o':
        return scanner_check_keyword(2, _("r"), TOKEN_FOR);
      case 'u':
        return scanner_check_keyword(2, _("n"), TOKEN_FUN);
      }
    }
    break;
  case 'i':
    return scanner_check_keyword(1, _("f"), TOKEN_IF);
  case 'n':
    return scanner_check_keyword(1, _("il"), TOKEN_NIL);
  case 'o':
    return scanner_check_keyword(1, _("r"), TOKEN_OR);
  case 'p':
    return scanner_check_keyword(1, _("rint"), TOKEN_PRINT);
  case 'r':
    return scanner_check_keyword(1, _("eturn"), TOKEN_RETURN);
  case 's':
    return scanner_check_keyword(1, _("uper"), TOKEN_SUPER);
  case 't':
    if (scanner.current - scanner.start > 1) {
      switch (scanner.start[1]) {
      case 'h':
        return scanner_check_keyword(2, _("is"), TOKEN_THIS);
      case 'r':
        return scanner_check_keyword(2, _("ue"), TOKEN_TRUE);
      }
    }
    break;
  case 'v':
    return scanner_check_keyword(1, _("ar"), TOKEN_VAR);
  case 'w':
    return scanner_check_keyword(1, _("hile"), TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

token_t scanner_identifier() {
  while (isalpha(scanner_peek()) || isdigit(scanner_peek()))
    scanner_advance();
  return scanner_make_token(scanner_identifier_type());
}

token_t scanner_error_token(const string_t message) {
  const token_t token = {
      .type = TOKEN_ERROR,
      .lexeme = message,
      .line = scanner.line,
  };
  return token;
}

const char scanner_advance() {
  const char chr = *scanner.current++;
  return chr;
}

bool scanner_match(const char chr) {
  if (scanner_is_end())
    return false;
  if (*scanner.current != chr)
    return false;
  scanner.current++;
  return true;
}

bool scanner_is_whitespace(const char chr) {
  return chr == ' ' || chr == '\r' || chr == '\t';
}

void scanner_skip_whitespace() {
  while (scanner_is_whitespace(scanner_peek()) && !scanner_is_end())
    scanner_advance();
}

const char scanner_peek() { return *scanner.current; }

const char scanner_peek_next() {
  if (scanner_is_end())
    return '\0';
  return scanner.current[1];
}
