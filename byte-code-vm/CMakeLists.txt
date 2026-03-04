cmake_minimum_required(VERSION 3.31)
project(ByteCodeVM C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
  add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_run
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests ..."
  )
endif()

find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
  file(GLOB_RECURSE ALL_SOURCE_FILES "src/*.*" "include/*.*")

  add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-format"
        VERBATIM
    )
else()
  message(WARNING "clang-format not found")
endif()

