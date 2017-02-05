#pragma once

#include <memory>
#include <queue>
#include <stack>

namespace tencatl {
  
enum class Token {
  NoToken,
  Variable,
  Symbol,
  EndOfExpression,
  BlockBegin,
  BlockEnd,
  EndOfFile
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
  /// Current input filename.
  std::string filename;
  
  /// Current character being read.
  char c;
  /// Whether we are at the beginning of a line.
  bool bol = true;
  /// Whether the end of file was read.
  bool eofed = false;
  /// Braces context.
  std::stack<char> braces;
  // Indentation stack.
  std::stack<std::size_t> indent;
  // Number of closing blocks to be generated
  std::size_t closing_blocks;
  // Tokens to be emitted in the future.
  std::queue<Token> to_emit;
  bool maybe_expend = false;
  std::size_t skipped_blocks = 0;
  
  /// Last returned token.
  Token last_token = Token::NoToken;
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
