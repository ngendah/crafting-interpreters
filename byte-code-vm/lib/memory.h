#pragma once

#include <stddef.h>
#include <stdint.h>

constexpr auto GROW_RATE = 2u;
constexpr auto MIN_ARRAY_LEN = 8u;

void *new (size_t size);
char *new_string(size_t size);
void *resize(void *pointer, size_t size);
void delete (void *pointer);
