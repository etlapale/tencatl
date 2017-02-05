#include <iostream>

#include <tencatl/lexer.h>

int main(int argc, char* argv[])
{
  using tencatl::Token;

  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " input.te" << std::endl;
    return 1;
  }
  
  tencatl::Lexer lexer(argv[1]);
  
  Token token;
  do {
    token = lexer.read_token();
    switch (token) {
    case Token::VARIABLE:
      std::cout << "<var>" << lexer.variable_name() << "</var>" << std::endl;
      break;
    case Token::SYMBOL:
      std::cout << "<sym>" << lexer.symbol() << "</sym>" << std::endl;
      break;
    case Token::END_OF_EXPRESSION:
      std::cout << std::endl;
      break;
    default:
      std::cerr << "Unknown token!" << std::endl;
    }
  } while (token != Token::END_OF_FILE
	   && token != Token::ERROR);
  
  return 0;
}
