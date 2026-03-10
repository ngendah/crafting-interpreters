#pragma once

#include "chunk.h"
#include "string.h"
#include "value.h"

bool compiler_run(const string_t source, chunk_t *chunk,
                  hash_table_t *hash_table);
