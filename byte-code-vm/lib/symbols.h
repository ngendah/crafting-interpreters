#pragma once

#include "common.h"
#include "hash_table.h"

typedef enum {
  SYMBOL_HIDDEN = 0x0,
  SYMBOL_VISIBLE = 0x01,
} symbol_visible_t;

constexpr auto SYMBOLS_SIZE = 256u;

typedef struct {
  value_t symbol, value;
  symbol_visible_t visible;
} symbol_t;

typedef struct {
  hash_table_t *hash_table;
  symbol_t symbols[SYMBOLS_SIZE];
  offset_t top;
} symbols_t;

void symbols_init(symbols_t *symbols, hash_table_t *hash_table);
void symbols_free(symbols_t *symbols);

bool symbols_exist(symbols_t *symbols, const string_t symbol);
offset_t symbols_add(symbols_t *symbols, const string_t symbol,
                     symbol_visible_t visible);
int32_t symbols_get_offset(symbols_t *symbols, const string_t symbol,
                           symbol_visible_t visible);
symbol_t *symbols_get_at(symbols_t *symbols, const offset_t at);

void symbols_symbol_set_visible(symbols_t *symbols, const offset_t at);
