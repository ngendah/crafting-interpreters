#pragma once

#include "common.h"
#include "hash_table.h"
#include "variables.h"
#include <stddef.h>

constexpr auto MAX_SCOPE_SIZE = 256u;

typedef struct {
  hash_table_t *hash_table;
  variables_t *variables;
} scope_t;

void scope_init(scope_t *environment);
void scope_free(scope_t *environment);

typedef struct {
  scope_t scopes[MAX_SCOPE_SIZE];
  offset_t count;
} environment_t;

void environment_init(environment_t *environment);
void environment_free(environment_t *environment);

void environment_scope_add(environment_t *environment);
