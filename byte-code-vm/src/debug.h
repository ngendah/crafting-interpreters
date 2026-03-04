#pragma once

#include "chunk.h"
#include "common.h"

Offset disassembleChunk(Chunk *chunk, const char *name);
Offset disassambleInstruction(Chunk *chunk, Offset offset);
