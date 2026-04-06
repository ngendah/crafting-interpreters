#include <stdlib.h>

#include "memory.h"

void *new(size_t size) { return malloc(size); }

char *new_string(size_t len) { return calloc(sizeof(char), len); }

void *resize(void *pointer, size_t size) { return realloc(pointer, size); }

void delete(void *pointer) { free(pointer); }
