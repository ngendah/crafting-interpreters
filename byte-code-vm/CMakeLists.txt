cmake_minimum_required(VERSION 3.31)
project(ByteCodeVM C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_library(LIBM m DOC "Math library")

if(NOT LIBM)
  message(FATAL_ERROR "libm math library required")
endif()

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(src)

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

