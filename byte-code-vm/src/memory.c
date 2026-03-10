#include <stdlib.h>

#include "memory.h"

void *new(size_t size) { return malloc(size); }

void *resize(void *pointer, size_t size) { return realloc(pointer, size); }

void delete(void *pointer) { free(pointer); }
