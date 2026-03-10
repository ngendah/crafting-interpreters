#pragma once

#include "chunk.h"
#include "common.h"

offset_t debug_chunk(chunk_t *chunk, const char *name);
offset_t debug_instruction(chunk_t *chunk, offset_t offset);
