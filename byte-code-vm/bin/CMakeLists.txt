include_directories("${PROJECT_SOURCE_DIR}/lib")

add_executable(main main.c)
target_link_libraries(main ByteCodeVM)
target_include_directories(main PRIVATE "${PROJECT_SOURCE_DIR}/lib")

add_custom_target(
  test_file ALL
  COMMAND cp --update=older "${PROJECT_SOURCE_DIR}/bin/main.lox"
              "${PROJECT_BINARY_DIR}/bin/main.lox"
  COMMENT "lox test source"
  VERBATIM
)
