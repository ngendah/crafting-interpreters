include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(parser main.cpp)
target_link_libraries(parser pratt_parser)
target_include_directories(parser PRIVATE "${PROJECT_SOURCE_DIR}/src")
