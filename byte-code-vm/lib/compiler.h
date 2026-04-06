#pragma once

#include "chunk.h"
#include "environment.h"

bool compiler_run(const string_t source, chunk_t *chunk,
                  environment_t *environment);
