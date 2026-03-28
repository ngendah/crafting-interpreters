set(CTEST_OUTPUT_ON_FAILURE 1)
include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(test_includes INTERFACE)
target_include_directories(test_includes INTERFACE "${PROJECT_SOURCE_DIR}/src")

add_executable(test_scanner scanner_test.cpp)
target_link_libraries(test_scanner PRIVATE pratt_parser test_includes)
add_test(NAME test_scanner COMMAND test_scanner)


