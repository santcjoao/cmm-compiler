#include "../include/Scanner.h"

Scanner::Scanner(Logger *log) {
  logger = log;
  input = logger->getSource();
  pos = 0;
  line = 1;
  column = 1;
  tokenColumn = 1;
}

// ==========================================
// Setup & Utilitários
// ==========================================
int Scanner::getLine() { return line; }
int Scanner::getTokenColumn() { return tokenColumn; }
void Scanner::lexicalError(int errLine, int errCol, string msg, int len) {
  logger->printDiagnostic(errLine, errCol, msg, len);
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
Token *Scanner::nextToken() {
  skipWhitespaceAndComments();
  if (isAtEnd())
    return makeToken(END_OF_FILE);

  tokenColumn = column;
  char c = currentPeek();

  if (isalpha(c))
    return readIdentifier();

  if (isdigit(c))
    return readNumber();

  if (c == '\'') {
    Token *t = readCharLiteral();
    return t ? t : nextToken();
  }

  if (c == '"') {
    Token *t = readStringLiteral();
    return t ? t : nextToken();
  }

  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '!' ||
      c == '<' || c == '>' || c == '&' || c == '|') {
    Token *t = readOperator();
    return t ? t : nextToken();
  }

  if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
      c == ',' || c == ';') {
    Token *t = readSeparator();
    return t ? t : nextToken();
  }

  string msg = "caractere não reconhecido: '";
  msg += c;
  msg += "'";
  lexicalError(line, tokenColumn, msg);
  advance();
  return nextToken();
}

// ==========================================
// Leitura de Tokens
// ==========================================
Token *Scanner::readNumber() {
  int startPos = pos;
  while (isdigit(currentPeek()))
    advance();
  return makeToken(INTEGERCONSTANT, UNDEF, extractLexeme(startPos, pos));
}

Token *Scanner::readIdentifier() {
  int startPos = pos;
  while (isalpha(currentPeek()) || isdigit(currentPeek()) ||
         currentPeek() == '_')
    advance();
  return makeToken(ID, UNDEF, extractLexeme(startPos, pos));
}

Token *Scanner::readCharLiteral() {
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
        return makeToken(CHARCONSTANT, BN, "\\n");
      }
    } else if (matchCurrent('0')) {
      advance();
      if (matchCurrent('\'')) {
        advance();
        return makeToken(CHARCONSTANT, B0, "\\0");
      }
    } else {
      lexicalError(startLine, startCol,
                   "tratamento de escape de caractere inválido.", 2);
      return nullptr;
    }
  } else if (isprint(currentPeek()) && !matchCurrent('\'') &&
             !matchCurrent('\\')) {
    advance();
    if (matchCurrent('\'')) {
      advance();
      return makeToken(CHARCONSTANT, CH, extractLexeme(startPos + 1, pos - 1));
    }
  }

  lexicalError(startLine, startCol, "literal de caractere mal formado.");
  return nullptr;
}

Token *Scanner::readStringLiteral() {
  int startPos = pos;
  int startLine = line;
  int startCol = column;
  advance();

  while (!isAtEnd() && !matchCurrent('"'))
    advance();

  if (isAtEnd()) {
    lexicalError(startLine, startCol,
                 "string não fechada antes do fim do arquivo.");
    return nullptr;
  }

  advance();
  return makeToken(STRINGCONSTANT, UNDEF, extractLexeme(startPos + 1, pos - 1));
}

Token *Scanner::readOperator() {
  char c = currentPeek();
  int startCol = column;
  advance();

  switch (c) {
  case '+':
    return makeToken(PLUS, UNDEF, "+");
  case '-':
    return makeToken(MINUS, UNDEF, "-");
  case '*':
    return makeToken(TIMES, UNDEF, "*");
  case '/':
    return makeToken(DIVIDED, UNDEF, "/");

  case '=':
    if (matchCurrent('=')) {
      advance();
      return makeToken(EQUIVALENT, UNDEF, "==");
    }
    return makeToken(ASSIGN, UNDEF, "=");

  case '!':
    if (matchCurrent('=')) {
      advance();
      return makeToken(DIFF, UNDEF, "!=");
    }
    return makeToken(NOT, UNDEF, "!");

  case '<':
    if (matchCurrent('=')) {
      advance();
      return makeToken(LE, UNDEF, "<=");
    }
    return makeToken(LT, UNDEF, "<");

  case '>':
    if (matchCurrent('=')) {
      advance();
      return makeToken(GE, UNDEF, ">=");
    }
    return makeToken(GT, UNDEF, ">");

  case '&':
    if (matchCurrent('&')) {
      advance();
      return makeToken(AND, UNDEF, "&&");
    }
    lexicalError(line, startCol, "operador '&' incompleto.");
    break;

  case '|':
    if (matchCurrent('|')) {
      advance();
      return makeToken(OR, UNDEF, "||");
    }
    lexicalError(line, startCol, "operador '|' incompleto.");
    break;

  default:
    lexicalError(line, startCol, "operador não reconhecido.");
    break;
  }

  return nullptr;
}

Token *Scanner::readSeparator() {
  char c = currentPeek();
  int startCol = column;
  advance();

  switch (c) {
  case '(':
    return makeToken(LPAREN, UNDEF, "(");
  case ')':
    return makeToken(RPAREN, UNDEF, ")");
  case '[':
    return makeToken(LBRACKET, UNDEF, "[");
  case ']':
    return makeToken(RBRACKET, UNDEF, "]");
  case '{':
    return makeToken(LBRACE, UNDEF, "{");
  case '}':
    return makeToken(RBRACE, UNDEF, "}");
  case ',':
    return makeToken(COMMA, UNDEF, ",");
  case ';':
    return makeToken(SEMICOLON, UNDEF, ";");
  default:
    lexicalError(line, startCol, "separador não reconhecido.");
    break;
  }

  return nullptr;
}