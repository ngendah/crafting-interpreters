set(CTEST_OUTPUT_ON_FAILURE 1)
include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(test_includes INTERFACE)
target_include_directories(test_includes INTERFACE "${PROJECT_SOURCE_DIR}/src")

add_executable(test_chunk chunk_test.c)
target_link_libraries(test_chunk PRIVATE ByteCodeVM test_includes)
add_test(NAME test_chunk COMMAND test_chunk)

add_executable(test_debug debug_test.c)
target_link_libraries(test_debug PRIVATE ByteCodeVM test_includes)
add_test(NAME test_debug COMMAND test_debug)

add_executable(test_vm vm_test.c)
target_link_libraries(test_vm PRIVATE ByteCodeVM test_includes)
add_test(NAME test_vm COMMAND test_vm)

add_executable(test_scanner scanner_test.c)
target_link_libraries(test_scanner PRIVATE ByteCodeVM test_includes)
add_test(NAME test_scanner COMMAND test_scanner)

add_executable(test_string string_test.c)
target_link_libraries(test_string PRIVATE ByteCodeVM test_includes)
add_test(NAME test_string COMMAND test_string)

add_executable(test_compiler compiler_test.c)
target_link_libraries(test_compiler PRIVATE ByteCodeVM test_includes)
add_test(NAME test_compiler COMMAND test_compiler)

add_executable(test_value value_test.c)
target_link_libraries(test_value PRIVATE ByteCodeVM test_includes)
add_test(NAME test_value COMMAND test_value)
