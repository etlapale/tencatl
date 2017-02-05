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
  std::string prefix;
  do {
    token = lexer.read_token();
    switch (token) {
    case Token::Variable:
      std::cout << prefix << "<var>" << lexer.variable_name() << "</var>" << std::endl;
      break;
    case Token::Symbol:
      std::cout << prefix << "<sym>" << lexer.symbol() << "</sym>" << std::endl;
      break;
    case Token::EndOfExpression:
      std::cout << std::endl;
      break;
    case Token::BlockBegin:
      prefix += "  ";
      break;
    default:
      std::cerr << "Unknown token!" << std::endl;
    }
  } while (token != Token::EndOfFile);
  
  return 0;
}
