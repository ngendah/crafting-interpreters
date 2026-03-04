add_executable(test_run chunk_test.c debug_test.c tests.c)
set(CTEST_OUTPUT_ON_FAILURE 1)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(test_includes INTERFACE)
target_include_directories(test_includes INTERFACE "${PROJECT_SOURCE_DIR}/src")

target_link_libraries(test_run PRIVATE ByteCodeVM test_includes)
add_test(NAME tests COMMAND test_run)

