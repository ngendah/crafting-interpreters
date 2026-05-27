cmake_minimum_required(VERSION 3.31)
project(ByteCodeVM C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Debug)

find_library(LIBM m DOC "Math library")

if(NOT LIBM)
  message(FATAL_ERROR "libm math library required")
endif()

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(lib)
add_subdirectory(bin)
