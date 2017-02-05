#pragma once

#include <memory>
#include <stack>

namespace tencatl {
  
enum class Token {
  VARIABLE,
  SYMBOL,
  END_OF_EXPRESSION,
  END_OF_FILE,
  ERROR,
  NO_TOKEN
};

class Lexer
{
public:
  /**
   * Create a lexer for the given input file.
   */
  Lexer(const std::string& path);

  /// Read the next token from the input stream.
  Token read_token();

  /// Return the last read token type.
  Token current_token() const
  { return last_token; }

  const std::string& variable_name() const
  { return last_var; }

  char symbol() const
  { return last_sym; }
  
private:
  //// Input stream.
  std::unique_ptr<std::istream> is;
  /// Current character being read.
  char c;
  /// Current input filename.
  std::string filename;
  /// Whether we are at the beginning of a line.
  bool bol = true;
  /// Whether the end of file was read.
  bool eofed = false;
  /// Braces context.
  std::stack<char> braces;
  /// Last returned token.
  Token last_token = Token::NO_TOKEN;
  /// Name of the last variable token.
  std::string last_var;
  /// Name of the last variable token.
  char last_sym;

  char opening_brace[128];

  /**
   * Read one character from the input.
   *
   * The read character is stored into the private variable ‘c’ and
   * also returned by value.
   */
  char get_char();
};

} // namespace tencatl
