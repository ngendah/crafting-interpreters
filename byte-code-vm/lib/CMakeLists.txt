add_library(ByteCodeVM chunk.c compiler.c
                      environment.c hash_table.c
                      memory.c scanner.c
                      stack.c string_utils.c
                      value.c variables.c vm.c)
target_compile_definitions(ByteCodeVM PUBLIC $<$<CONFIG:Debug>:DEBUG_ENABLED>)
target_link_libraries(ByteCodeVM m)

