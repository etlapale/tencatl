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

static bool isop(char c)
{
  return c == '+' || c == '-' || c == '/' || c == '*' || c == '%'
    || c == '<' || c == '>' || c == '=' || c == '!'
    || c == '&' || c == '|'
    || c == ':';
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
  //std::cout << "  << Read ‘" << c << "’" << std::endl;
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
    return last_token = tok;
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
	return last_token = Token::BlockEnd;
      }
    }

  // Skip whitespaces
  while (!bol && isblank(c))
    get_char();
  
  // Skip empty lines
  if (bol && c == '\n')
    get_char();

  // Close all the indentation blocks
  if (indent.size()
      && ((bol && c != ' ') || (is->eof() && indent.size()))) {
    closing_blocks = indent.size();
    
    while (indent.size()) indent.pop();
    bol = false;
    
    return read_token();
  }
  
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
	    to_emit.push(Token::EndOfExpression);
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
  
  // Number litterals
  if (std::isdigit(c) || c == '.') {

    auto first_char = c;
    std::stringstream buf;
    
    bool dotted = false;
    while (std::isdigit(c) || ((!dotted) && c == '.')) {
      dotted = dotted || (c == '.');
      buf << c;
      get_char();
    }

    // Just a dot
    if (buf.tellg() == 1 && first_char == '.') {
      last_sym = '.';
      return last_token = Token::Symbol;
    }

    // Hexadecimal numbers (starting with 0x)
    int base = 10;
    if (buf.tellg () == 1 && first_char == '0' && c == 'x') {
      base = 16;
      while (isxdigit(get_char()))
	buf << c;
    }
    
    // Long integers ends with `L'
    bool is_long = false;
    if (!dotted && c == 'L') {
      is_long = true;
      get_char();
    }

    auto lit = buf.str();
    
    // Check the number type
    if (dotted) {
      last_float = std::stod(lit);
      return Token::Double;
    }
    else if (is_long) {
      last_int = std::stol(lit);
      return Token::Long;
    }
    else {
      last_int = std::stoi(lit);
      return Token::Int;
    }
  }
  
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
  
  // Operator
  if (isop(c)) {
    std::stringstream buf;
    buf << c;
    while (isop(get_char()))
      buf << c;
    auto name = buf.str();
    
    // Special cases
    if (name.size() == 1
	&& std::string("=:+-~!").find(name[0]) != std::string::npos) {
      last_sym = name[0];
      return last_token = Token::Symbol;
    }
    if (name == ";")
      return last_token = Token::EndOfExpression;

    last_oper = name;
    return last_token = Token::Operator;
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
	|| last_token == Token::EndOfExpression
	|| last_token == Token::BlockBegin
	|| last_token == Token::BlockEnd)
      return read_token();

    // Avoid EndOfExpression on BlockEnd
    if (bol && c == ' ') {
      maybe_expend = true;
      return read_token();
    }

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
