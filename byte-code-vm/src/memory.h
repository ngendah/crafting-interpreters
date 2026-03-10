#pragma once

#include <stddef.h>
#include <stdint.h>

#define GROW_RATE (int)2
#define MIN_ARRAY_LEN (size_t)8

void *new (size_t size);
void *resize(void *pointer, size_t size);
void delete (void *pointer);
