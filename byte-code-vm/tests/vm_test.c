#include <assert.h>
#include <stdlib.h>

#include "chunk.h"
#include "common.h"
#include "vm.h"

extern vm_t vm;
extern bool vm_is_end();
extern byte_t vm_instruction_next();
extern vm_result_t vm_execute();
extern void vm_add();
extern void vm_subtract();

void test_vm_is_end() {
  vm_init();
  {
    assert(vm_is_end() == true);
  }
  vm_free();
}

void test_add_operation() {
  vm_init();
  {
    stack_push(&vm.stack, value_from_number(1));
    stack_push(&vm.stack, value_from_number(2));
    chunk_write(vm.chunk, OP_ADD, 1);
    vm_add();
    value_t value = stack_peek(&vm.stack);
    assert(value_as_number(value) == 3);
  }
  vm_free();
}

void test_subtract_operation() {
  vm_init();
  {
    chunk_write(vm.chunk, OP_CONSTANT, 1);
    offset_t offset = chunk_add_constant(vm.chunk, value_from_number(5));
    chunk_write(vm.chunk, offset, 1);
    chunk_write(vm.chunk, OP_CONSTANT, 1);
    offset = chunk_add_constant(vm.chunk, value_from_number(2));
    chunk_write(vm.chunk, offset, 1);
    chunk_write(vm.chunk, OP_SUBTRACT, 1);
    vm_execute();
    value_t value = stack_peek(&vm.stack);
    assert(value_as_number(value) == 3);
  }
  vm_free();
}

void test_block_statement() {
  vm_init();
  auto source = _("{"
                  " print \"2\";"
                  "}");
  auto result = vm_run(source);
  vm_free();
  assert(result == OK);
}

void test_if_statement() {
  vm_init();
  auto source = _("if(true){"
                  " print \"true\";"
                  "}");
  auto result = vm_run(source);
  vm_free();
  assert(result == OK);
}

void test_variable_statements() {
  vm_init();
  auto source = _("\n"
                  "print \"clox program\";\n"
                  "var mytest;\n"
                  "mytest=0;\n"
                  "{\n"
                  "  var myvar;\n"
                  "  myvar = 123;\n"
                  "  {\n"
                  "    var myvar=234;\n"
                  "    print myvar;\n"
                  "    mytest=2;\n"
                  "  }\n"
                  "  mytest = 1;\n"
                  "  print mytest;\n"
                  "  mytest = \"outer\";"
                  "  {\n"
                  "    var mytest=mytest;\n"
                  "    print mytest;\n"
                  "  }\n"
                  "}\n");
  auto result = vm_run(source);
  vm_free();
  assert(result == OK);
}

int main() {
  test_vm_is_end();
  test_add_operation();
  test_subtract_operation();
  test_block_statement();
  test_if_statement();
  test_variable_statements();
  return EXIT_SUCCESS;
}
