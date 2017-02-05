#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <tencatl/lexer.h>


namespace tencatl {

static const std::string allowed_symbols("()[]:");

static bool isvarstart(char c)
{
  return ('a' <= c && c <= 'z')
    || ('A' <= c && c <= 'Z')
    || c == '_';
}


static bool isvar(char c)
{
  return isvarstart(c)
    || ('0' <= c && c <= '9');
}


Lexer::Lexer(const std::string& path)
  : is(new std::ifstream(path))
{
  if (! is->good())
    throw std::runtime_error("could not open " + path);

  // Initialize the state
  filename = path;
  get_char();
  
  // Initialise the braces
  opening_brace[')'] = '(';
  opening_brace[']'] = '[';
  opening_brace['}'] = '{';
}


char Lexer::get_char()
{
  is->get(c);
  //std::cout << "Read ‘" << c << "’" << std::endl;
  return c;
}


Token Lexer::read_token()
{
  // Skip whitespaces
  while (!bol && isblank(c))
    get_char();
  
  // Skip empty lines
  if (bol && c == '\n')
    c = get_char();

  bol = false;

  // Potential variable
  if (isvarstart(c)) {
    // Read the potential variable
    std::stringstream buf;
    buf << c;
    while (isvar(get_char()))
      buf << c;

    last_var = buf.str();
    return last_token = Token::VARIABLE;
  }
  
  // Handle end of files
  if (is->eof()) {
    // Ensure that applications do not try to read passed the EOF
    if (eofed)
      throw std::runtime_error("trying to get a token after the end of file");
    eofed = true;
    return last_token = Token::END_OF_FILE;
  }
  
  // Single line comments start with ‘#’ and go to end of line
  if (c == '#')
    while (get_char() != '\n');

  // End of line
  if (c == '\n') {
    bol = true;
    get_char();
    
    // Avoid generating empty expressions
    if (last_token == Token::NO_TOKEN
	|| last_token == Token::END_OF_EXPRESSION)
      return read_token();

    return last_token = Token::END_OF_EXPRESSION;
  }

  // Save braces contexts
  switch (c) {
  case '(':
  case '{':
  case '[':
    braces.push (c);
    break;
  case ')':
  case '}':
  case ']':
    if (braces.empty())
      throw std::runtime_error("found closing ‘"
			       + std::to_string(c)
			       + "’ without matching opening");
    auto o = opening_brace[static_cast<unsigned>(c)];
    auto t = braces.top();
    if (t != o)
      throw std::runtime_error("closing ‘"
			       + std::to_string(c)
			       + "’ does not match opening ‘"
			       + t + "’");
    braces.pop();
    break;
  }

  // The character is itself a symbol
  if (allowed_symbols.find(c) != std::string::npos) {
    // Save the symbol
    last_sym = c;

    // Set the state with the next character
    get_char();
    
    return last_token = Token::SYMBOL;
  }

  // Unknown character
  throw std::runtime_error(std::string("invalid character ‘")
			   + c + "’");
}

} // namespace tencatl
