add_library(ByteCodeVM chunk.c compiler.c debug.c memory.c scanner.c string.c value.c vm.c)
target_link_libraries(ByteCodeVM m)
