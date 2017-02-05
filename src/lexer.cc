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
  // Emit tokens in the queue
  while (! to_emit.empty()) {
    auto tok = to_emit.front();
    switch (tok)
      {
	// Tokens allowed in to_emit queue
      case Token::BlockBegin:
      case Token::BlockEnd:
      case Token::EndOfExpression:
	break;
      default:
	throw std::runtime_error("invalid token in to-emit queue");
      }
    to_emit.pop();
    return tok;
  }

  // Closing blocks
  if (closing_blocks != 0)
    {
      closing_blocks--;

      // Since BlockBegin are ignored inside braces, also
      // ignore the corresponding BlockEnd
      if (skipped_blocks > 0) {
	skipped_blocks--;
      }
      else {
	return Token::BlockEnd;
      }
    }

  // Skip whitespaces
  while (!bol && isblank(c))
    get_char();
  
  // Skip empty lines
  if (bol && c == '\n')
    c = get_char();
  
  // Indentation: only spaces
  if (bol && c == ' ') {
    bol = false;
    
    // Calculate indentation length
    std::size_t l;
    for (l = 1; get_char() == ' '; l++);
    
    // Ignore indented comments
    if (c != '#') {
      // Starts an inner block
      if (indent.size() == 0 || indent.top() < l) {
	indent.push(l);
	
	// Ignore blocks inside braces
	if (braces.empty())
	  return Token::BlockBegin;
	else
	  skipped_blocks++;
      }
      // Ends an inner block
      else if (indent.top() > l) {
	  indent.pop();
	  closing_blocks = 1;
	  
	  while (! indent.empty() && indent.top() > l) {
	    indent.pop();
	    closing_blocks++;
	  }
	  
	  if (indent.empty() || indent.top() != l)
	    throw std::runtime_error("bad indentation");
	  
	  // Emit an EndOfExpression on BlockEnd if necessary
	  if (maybe_expend) {
	    maybe_expend = false;
	    to_emit.push(Token::BlockBegin);
	  }
	  
	  return read_token();
	}
      // Same-level indentation
      else {
	if (maybe_expend) {
	  maybe_expend = false;
	  
	  to_emit.push(Token::EndOfExpression);
	  return read_token();
	}
      }
    }
  }
  bol = false;

  // Potential variable
  if (isvarstart(c)) {
    // Read the potential variable
    std::stringstream buf;
    buf << c;
    while (isvar(get_char()))
      buf << c;

    last_var = buf.str();
    return last_token = Token::Variable;
  }
  
  // Handle end of files
  if (is->eof()) {
    // Ensure that applications do not try to read passed the EOF
    if (eofed)
      throw std::runtime_error("trying to get a token after the end of file");
    eofed = true;
    return last_token = Token::EndOfFile;
  }
  
  // Single line comments start with ‘#’ and go to end of line
  if (c == '#')
    while (get_char() != '\n');

  // End of line
  if (c == '\n') {
    bol = true;
    get_char();
    
    // Avoid generating empty expressions
    if (last_token == Token::NoToken
	|| last_token == Token::EndOfExpression)
      return read_token();

    return last_token = Token::EndOfExpression;
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
    
    return last_token = Token::Symbol;
  }

  // Unknown character
  throw std::runtime_error(std::string("invalid character ‘")
			   + c + "’");
}

} // namespace tencatl
