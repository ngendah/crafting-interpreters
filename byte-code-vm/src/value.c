#include <stddef.h>
#include <stdio.h>

#include "memory.h"
#include "value.h"

void initValues(Values *values) {
  values->values = NULL;
  values->capacity = 0;
  values->count = 0;
}

void writeValue(Values *values, Value value) {
  if (values->capacity < values->count + 1) {
    int oldCapacity = values->capacity;
    values->capacity = GROW_CAPACITY(oldCapacity);
    values->values =
        GROW_ARRAY(Value, values->values, oldCapacity, values->capacity);
  }
  values->values[values->count] = value;
  values->count++;
}

void freeValues(Values *values) {
  FREE_ARRAY(Value, values->values, values->capacity);
  initValues(values);
}

void printValue(Value value) { printf("%g", value); }
