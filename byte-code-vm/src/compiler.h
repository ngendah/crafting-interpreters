#pragma once

#include "chunk.h"
#include "string.h"

bool compiler_run(const string_t source, chunk_t *chunk);
