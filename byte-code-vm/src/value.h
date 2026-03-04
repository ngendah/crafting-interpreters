#pragma once

typedef double Value;

typedef struct {
  int capacity;
  int count;
  Value *values;
} Values;

void initValues(Values *values);
void writeValue(Values *values, Value value);
void freeValues(Values *values);
void printValue(Value value);
