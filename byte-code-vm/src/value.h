#pragma once

#include <stddef.h>

typedef double Value;

typedef struct {
  size_t capacity;
  int count;
  Value *values;
} Values;

void initValues(Values *values);
void writeValue(Values *values, Value value);
void freeValues(Values *values);
void printValue(Value value);
