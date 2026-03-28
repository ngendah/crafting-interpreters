cmake_minimum_required(VERSION 3.31)
project(PrattParser CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(bin)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
