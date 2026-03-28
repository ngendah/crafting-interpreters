add_library(pratt_parser scanner.cpp parser.cpp)

target_link_libraries(pratt_parser fmt)
