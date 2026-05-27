#include <assert.h>
#include <string.h>

#include "common.h"
#include "symbols.h"
#include "value.h"

void symbols_init(symbols_t *symbols, hash_table_t *hash_table) {
  symbols->hash_table = hash_table;
  symbols->top = 0;
  memset(symbols->symbols, 0, SYMBOLS_SIZE * sizeof(value_t));
}

void symbols_free(symbols_t *symbols) {
  symbols->top = 0;
  symbols->hash_table = nullptr;
  memset(symbols->symbols, 0, SYMBOLS_SIZE * sizeof(value_t));
}

offset_t symbols_add(symbols_t *symbols, const string_t str,
                     symbol_visible_t visible) {
  const auto symbol = value_from_string(str, symbols->hash_table);
  const auto top = symbols->top;
  symbols->symbols[top] =
      (symbol_t){.symbol = symbol, .value = value_nil(), .visible = visible};
  symbols->top++;
  return top;
}

bool symbols_exist(symbols_t *symbols, const string_t symbol) {
  const auto value = value_from_string(symbol, symbols->hash_table);
  auto it = &symbols->symbols[0];
  const auto end = &symbols->symbols[symbols->top];
  while (it != end) {
    if (value_compare(it->value, value)) {
      return true;
    }
    it++;
  }
  return false;
}

int32_t symbols_get_offset(symbols_t *symbols, const string_t symbol,
                           symbol_visible_t visible) {
  const auto _name = value_from_string(symbol, symbols->hash_table);
  const auto vars = symbols->symbols;
  auto it = 0;
  const auto end = symbols->top;
  while (it < end) {
    const auto variable = vars[it];
    if (variable.visible == visible && value_compare(variable.symbol, _name)) {
      return it;
    }
    it++;
  }
  return -1;
}

symbol_t *symbols_get_at(symbols_t *symbols, offset_t at) {
  assert(at < symbols->top);
  return &symbols->symbols[at];
}

void symbols_symbol_set_visible(symbols_t *symbols, const offset_t at) {
  assert(at < symbols->top);
  symbols->symbols[at].visible = SYMBOL_VISIBLE;
}
