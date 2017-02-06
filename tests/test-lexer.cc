#include <memory>

#include <gtest/gtest.h>

#include <tencatl/lexer.h>


TEST(TestLexer, CreateForFile)
{
  tencatl::Lexer lexer(TENCATL_TEST_DIR "fragments/empty.te");
}

TEST(TestLexer, LexEmptyFile)
{
  tencatl::Lexer lexer(TENCATL_TEST_DIR "fragments/empty.te");

  // Lexing and empty file should return EOF
  auto token = lexer.read_token();
  EXPECT_EQ(token, tencatl::Token::EndOfFile);

  // Reading after the end of file should throw
  EXPECT_THROW(lexer.read_token(), std::runtime_error);
}

TEST(TestLexer, LexEmptyString)
{
  std::string input("");
  tencatl::Lexer lexer;
  lexer.set_source(std::make_unique<std::istringstream>(input));

  // Lexing and empty file should return EOF
  auto token = lexer.read_token();
  EXPECT_EQ(token, tencatl::Token::EndOfFile);

  // Reading after the end of file should throw
  EXPECT_THROW(lexer.read_token(), std::runtime_error);
}
