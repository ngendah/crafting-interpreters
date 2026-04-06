#pragma once

#include "common.h"
#include "hash_table.h"

typedef enum {
  VAR_HIDDEN = 0x0,
  VAR_VISIBLE = 0x01,
} var_visible_t;

constexpr auto VARIABLE_SIZE = 256u;

typedef struct {
  value_t name, value;
  var_visible_t visible;
} variable_t;

typedef struct {
  hash_table_t *hash_table;
  variable_t variables[VARIABLE_SIZE];
  offset_t top;
} variables_t;

void variables_init(variables_t *variables, hash_table_t *hash_table);
void variables_free(variables_t *variables);

bool variables_exist(variables_t *variables, const string_t name);
offset_t variables_add(variables_t *variables, const string_t name,
                       var_visible_t visible);
int32_t variables_get_offset(variables_t *variables, const string_t name,
                             var_visible_t visible);
variable_t *variables_get_at(variables_t *variables, const offset_t at);

void variables_variable_set_visible(variables_t *variables, const offset_t at);
