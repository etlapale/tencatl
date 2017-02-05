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
    case Token::Double:
      std::cout << prefix << "<double>" << lexer.float_value() << "</double>" << std::endl;
      break;
    case Token::Long:
      std::cout << prefix << "<long>" << lexer.int_value() << "</long>" << std::endl;
      break;
    case Token::Int:
      std::cout << prefix << "<int>" << lexer.int_value() << "</int>" << std::endl;
      break;
    case Token::String:
      std::cout << prefix << "<str>" << lexer.string() << "</str>" << std::endl;
      break;
    case Token::Variable:
      std::cout << prefix << "<var>" << lexer.variable_name() << "</var>" << std::endl;
      break;
    case Token::Symbol:
      std::cout << prefix << "<sym>" << lexer.symbol() << "</sym>" << std::endl;
      break;
    case Token::Operator:
      std::cout << prefix << "<op>" << lexer.oper() << "</op>" << std::endl;
      break;
    case Token::EndOfExpression:
      std::cout << prefix << "<expend/>\n" << std::endl;
      break;
    case Token::BlockBegin:
      prefix += "  ";
      break;
    case Token::BlockEnd:
      prefix = prefix.substr(2);
      break;
    case Token::EndOfFile:
      break;
    default:
      std::cerr << "Unknown token!" << std::endl;
    }
  } while (token != Token::EndOfFile);
  
  return 0;
}
