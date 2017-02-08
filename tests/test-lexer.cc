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

TEST(TestLexer, LexFib)
{
  using tencatl::Token;
  
  std::string input("# Recursively define the Fibonacci function\n"
		    "def fib (x : int)\n"
		    "  if x < 3\n"
		    "    1\n"
		    "  else\n"
		    "    fib (x - 1) + fib (x - 2)\n");
  tencatl::Lexer lexer;
  lexer.set_source(std::make_unique<std::istringstream>(input));

  std::vector<tencatl::Token> truth {
    // def · fib · (
    Token::Variable, Token::Variable, Token::Symbol,
    // x · : · int
    Token::Variable, Token::Symbol, Token::Variable,
    // ) · → · if
    Token::Symbol, Token::BlockBegin, Token::Variable,
    // x · < · 3
    Token::Variable, Token::Operator, Token::Int,
    // → · 1 · ;
    Token::BlockBegin, Token::Int, Token::EndOfExpression,
    // ← · else · →
    Token::BlockEnd, Token::Variable, Token::BlockBegin,
    // fib · ( · x
    Token::Variable, Token::Symbol, Token::Variable
  };
    
  // Check the tokens agains ground truth
  std::vector<tencatl::Token> tokens;
  tencatl::Token token;
  do {
    token = lexer.read_token();
    tokens.push_back(token);
  } while (token != tencatl::Token::EndOfFile);
  ASSERT_GE(tokens.size(), truth.size());
  EXPECT_TRUE(equal(truth.cbegin(), truth.cend(), tokens.cbegin()));
}
