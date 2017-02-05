#include <boost/test/included/unit_test.hpp>

#define BOOST_TEST_MODULE Tēncatl Lexer Tests
#include <boost/test/included/unit_test.hpp>

#include <tencatl/lexer.h>

boost::unit_test::test_suite* init_unit_test_suite(int /*argc*/,
						   char* /*argv*/[])
{
  return 0;
}

BOOST_AUTO_TEST_CASE(test_lexer_create)
{
  tencatl::Lexer lexer(TENCATL_TEST_DIR "fragments/empty.te");
}

BOOST_AUTO_TEST_CASE(test_lexer_empty)
{
  tencatl::Lexer lexer(TENCATL_TEST_DIR "fragments/empty.te");

  // Lexing and empty file should return EOF
  auto token = lexer.read_token();
  BOOST_TEST(token == tencatl::Token::EndOfFile);

  // Reading after the end of file should throw
  BOOST_CHECK_THROW(lexer.read_token(), std::runtime_error);
}
