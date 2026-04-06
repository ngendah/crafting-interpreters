#include <assert.h>
#include <string.h>

#include "common.h"
#include "value.h"
#include "variables.h"

void variables_init(variables_t *variables, hash_table_t *hash_table) {
  variables->hash_table = hash_table;
  variables->top = 0;
  memset(variables->variables, 0, VARIABLE_SIZE * sizeof(value_t));
}

void variables_free(variables_t *variables) {
  variables->top = 0;
  variables->hash_table = nullptr;
  memset(variables->variables, 0, VARIABLE_SIZE * sizeof(value_t));
}

offset_t variables_add(variables_t *variables, const string_t str,
                       var_visible_t visible) {
  const auto name = value_from_string(str, variables->hash_table);
  const auto top = variables->top;
  variables->variables[top] =
      (variable_t){.name = name, .value = value_nil(), .visible = visible};
  variables->top++;
  return top;
}

bool variables_exist(variables_t *variables, const string_t name) {
  const auto value = value_from_string(name, variables->hash_table);
  auto it = &variables->variables[0];
  const auto end = &variables->variables[variables->top];
  while (it != end) {
    if (value_compare(it->value, value)) {
      return true;
    }
    it++;
  }
  return false;
}

int32_t variables_get_offset(variables_t *variables, const string_t name,
                             var_visible_t visible) {
  const auto _name = value_from_string(name, variables->hash_table);
  const auto vars = variables->variables;
  auto it = 0;
  const auto end = variables->top;
  while (it < end) {
    const auto variable = vars[it];
    if (variable.visible == visible && value_compare(variable.name, _name)) {
      return it;
    }
    it++;
  }
  return -1;
}

variable_t *variables_get_at(variables_t *variables, offset_t at) {
  assert(at < variables->top);
  return &variables->variables[at];
}

void variables_variable_set_visible(variables_t *variables, const offset_t at) {
  assert(at < variables->top);
  variables->variables[at].visible = VAR_VISIBLE;
}
