#include <assert.h>
#include <string.h>

#include "environment.h"
#include "memory.h"

void scope_init(scope_t *scope) {
  scope->hash_table = new(sizeof(hash_table_t));
  scope->symbols = new(sizeof(symbols_t));
  hash_table_init(scope->hash_table, value_free);
  symbols_init(scope->symbols, scope->hash_table);
}

void scope_free(scope_t *scope) {
  symbols_free(scope->symbols);
  hash_table_free(scope->hash_table);
}

// TODO: Better handled with a general stack

void environment_init(environment_t *environment) {
  environment->count = 0;
  memset(environment->scopes, 0, MAX_SCOPE_SIZE);
}

void environment_free(environment_t *environment) {
  auto it = &environment->scopes[0];
  while (it != &environment->scopes[environment->count]) {
    scope_free(it);
    it++;
  }
  environment_init(environment);
}

void environment_scope_add(environment_t *environment) {
  assert(environment->count < MAX_SCOPE_SIZE);
  const auto at = environment->count;
  scope_init(&environment->scopes[at]);
  environment->count++;
}
