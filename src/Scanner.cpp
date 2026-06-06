#include "../include/Scanner.h"
#include "../include/SourceFile.h"

#include <cctype>

using namespace std;

Scanner::Scanner(const SourceFile &sourceFile, Logger *log) {
  logger = log;
  input = sourceFile.getSource();
  pos = 0;
  line = 1;
  column = 1;
  tokenColumn = 1;
}

// ==========================================
// Setup & Utilitários
// ==========================================
int Scanner::getLine() const { return line; }
int Scanner::getTokenColumn() const { return tokenColumn; }

void Scanner::lexicalError(int errLine, int errCol, const string &msg,
                           int len) {
  logger->error(errLine, errCol, msg, len);
}

void Scanner::advance(int steps) {
  for (int i = 0; i < steps; i++) {
    if (pos < (int)input.length()) {
      if (input[pos] == '\n') {
        line++;
        column = 1;
      } else {
        column++;
      }
      pos++;
    }
  }
}

void Scanner::skipWhitespaceAndComments() {
  while (true) {
    char c = currentPeek();

    if (isspace(c)) {
      advance();
    } else if (c == '/' && nextPeek() == '/') {
      advance(2);
      while (!matchCurrent('\n') && !isAtEnd())
        advance();
      if (!isAtEnd())
        advance();
    } else if (c == '/' && nextPeek() == '*') {
      int startLine = line;
      int startCol = column;
      advance(2);
      while (!(matchCurrent('*') && matchNext('/')) && !isAtEnd())
        advance();
      if (!isAtEnd())
        advance(2);
      else
        lexicalError(startLine, startCol, "comentário de bloco não fechado.",
                     2);
    } else {
      break;
    }
  }
}

// ==========================================
// Principal
// ==========================================
Token Scanner::nextToken() {
  while (true) {
    skipWhitespaceAndComments();
    if (isAtEnd())
      return makeToken(TokenType::END_OF_FILE);

    tokenColumn = column;
    char c = currentPeek();

    if (isalpha(c))
      return readIdentifier();

    if (isdigit(c))
      return readNumber();

    if (c == '\'') {
      Token t;
      if (readCharLiteral(t))
        return t;
      continue;
    }

    if (c == '"') {
      Token t;
      if (readStringLiteral(t))
        return t;
      continue;
    }

    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '!' ||
        c == '<' || c == '>' || c == '&' || c == '|') {
      Token t;
      if (readOperator(t))
        return t;
      continue;
    }

    if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
        c == ',' || c == ';') {
      return readSeparator();
    }

    string msg = "caractere não reconhecido: '";
    msg += c;
    msg += "'";
    lexicalError(line, tokenColumn, msg);
    advance();
  }
}

// ==========================================
// Leitura de Tokens
// ==========================================
Token Scanner::readNumber() {
  int startPos = pos;
  while (isdigit(currentPeek()))
    advance();
  return makeToken(TokenType::INTEGERCONSTANT, TokenType::UNDEF,
                   extractLexeme(startPos, pos));
}

Token Scanner::readIdentifier() {
  int startPos = pos;
  while (isalpha(currentPeek()) || isdigit(currentPeek()) ||
         currentPeek() == '_')
    advance();
  return makeToken(TokenType::ID, TokenType::UNDEF,
                   extractLexeme(startPos, pos));
}

bool Scanner::readCharLiteral(Token &out) {
  int startPos = pos;
  int startLine = line;
  int startCol = column;
  advance();

  if (matchCurrent('\\')) {
    advance();
    if (matchCurrent('n')) {
      advance();
      if (matchCurrent('\'')) {
        advance();
        out = makeToken(TokenType::CHARCONSTANT, TokenType::BN, "\\n");
        return true;
      }
    } else if (matchCurrent('0')) {
      advance();
      if (matchCurrent('\'')) {
        advance();
        out = makeToken(TokenType::CHARCONSTANT, TokenType::B0, "\\0");
        return true;
      }
    } else {
      lexicalError(startLine, startCol,
                   "tratamento de escape de caractere inválido.", 2);
      return false;
    }
  } else if (isprint(currentPeek()) && !matchCurrent('\'') &&
             !matchCurrent('\\')) {
    advance();
    if (matchCurrent('\'')) {
      advance();
      out = makeToken(TokenType::CHARCONSTANT, TokenType::CH,
                      extractLexeme(startPos + 1, pos - 1));
      return true;
    }
  }

  lexicalError(startLine, startCol, "literal de caractere mal formado.");
  return false;
}

bool Scanner::readStringLiteral(Token &out) {
  int startPos = pos;
  int startLine = line;
  int startCol = column;
  advance();

  while (!isAtEnd() && !matchCurrent('"'))
    advance();

  if (isAtEnd()) {
    lexicalError(startLine, startCol,
                 "string não fechada antes do fim do arquivo.");
    return false;
  }

  advance();
  out = makeToken(TokenType::STRINGCONSTANT, TokenType::UNDEF,
                  extractLexeme(startPos + 1, pos - 1));
  return true;
}

bool Scanner::readOperator(Token &out) {
  char c = currentPeek();
  int startCol = column;
  advance();

  switch (c) {
  case '+':
    out = makeToken(TokenType::PLUS, TokenType::UNDEF, "+");
    return true;
  case '-':
    out = makeToken(TokenType::MINUS, TokenType::UNDEF, "-");
    return true;
  case '*':
    out = makeToken(TokenType::TIMES, TokenType::UNDEF, "*");
    return true;
  case '/':
    out = makeToken(TokenType::DIVIDED, TokenType::UNDEF, "/");
    return true;

  case '=':
    if (matchCurrent('=')) {
      advance();
      out = makeToken(TokenType::EQUIVALENT, TokenType::UNDEF, "==");
      return true;
    }
    out = makeToken(TokenType::ASSIGN, TokenType::UNDEF, "=");
    return true;

  case '!':
    if (matchCurrent('=')) {
      advance();
      out = makeToken(TokenType::DIFF, TokenType::UNDEF, "!=");
      return true;
    }
    out = makeToken(TokenType::NOT, TokenType::UNDEF, "!");
    return true;

  case '<':
    if (matchCurrent('=')) {
      advance();
      out = makeToken(TokenType::LE, TokenType::UNDEF, "<=");
      return true;
    }
    out = makeToken(TokenType::LT, TokenType::UNDEF, "<");
    return true;

  case '>':
    if (matchCurrent('=')) {
      advance();
      out = makeToken(TokenType::GE, TokenType::UNDEF, ">=");
      return true;
    }
    out = makeToken(TokenType::GT, TokenType::UNDEF, ">");
    return true;

  case '&':
    if (matchCurrent('&')) {
      advance();
      out = makeToken(TokenType::AND, TokenType::UNDEF, "&&");
      return true;
    }
    lexicalError(line, startCol, "operador '&' incompleto.");
    break;

  case '|':
    if (matchCurrent('|')) {
      advance();
      out = makeToken(TokenType::OR, TokenType::UNDEF, "||");
      return true;
    }
    lexicalError(line, startCol, "operador '|' incompleto.");
    break;

  default:
    lexicalError(line, startCol, "operador não reconhecido.");
    break;
  }

  return false;
}

Token Scanner::readSeparator() {
  char c = currentPeek();
  advance();

  switch (c) {
  case '(':
    return makeToken(TokenType::LPAREN, TokenType::UNDEF, "(");
  case ')':
    return makeToken(TokenType::RPAREN, TokenType::UNDEF, ")");
  case '[':
    return makeToken(TokenType::LBRACKET, TokenType::UNDEF, "[");
  case ']':
    return makeToken(TokenType::RBRACKET, TokenType::UNDEF, "]");
  case '{':
    return makeToken(TokenType::LBRACE, TokenType::UNDEF, "{");
  case '}':
    return makeToken(TokenType::RBRACE, TokenType::UNDEF, "}");
  case ',':
    return makeToken(TokenType::COMMA, TokenType::UNDEF, ",");
  case ';':
    return makeToken(TokenType::SEMICOLON, TokenType::UNDEF, ";");
  default:
    return makeToken(TokenType::UNDEF);
  }
}