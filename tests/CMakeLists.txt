add_executable(test-lexer test-lexer.cc)
target_link_libraries(test-lexer lexer GTest::GTest GTest::Main)
target_compile_definitions(test-lexer PRIVATE TENCATL_TEST_DIR="${CMAKE_SOURCE_DIR}/tests/")
add_test(NAME TestLexer COMMAND test-lexer)