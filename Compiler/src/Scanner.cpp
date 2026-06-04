#include "../include/Scanner.h"

Scanner::Scanner(string input) {
  pos = 0;
  line = 1;
  column = 1;
  tokenLine = 1;
  tokenColumn = 1;
  errorCount = 0;

  ifstream inputFile(input, ios::in);
  string line;

  if (inputFile.is_open()) {
    while (getline(inputFile, line)) {
      this->input.append(line + '\n');
    }
    inputFile.close();
  } else
    cout << "Unable to open file\n";
}

int Scanner::getLine() { return line; }
int Scanner::getColumn() { return column; }
int Scanner::getTokenLine() { return tokenLine; }
int Scanner::getTokenColumn() { return tokenColumn; }
int Scanner::getErrorCount() { return errorCount; }

string Scanner::getLineContent(int lineNumber) {
  int current = 1;
  int start = 0;
  for (int i = 0; i < input.length(); i++) {
    if (current == lineNumber) {
      int end = i;
      while (end < input.length() && input[end] != '\n') {
        end++;
      }
      return input.substr(start, end - start);
    }
    if (input[i] == '\n') {
      current++;
      start = i + 1;
    }
  }
  return "";
}
void Scanner::lexicalError(string msg) {
  cout << "arquivo.cmm:" << line << ":" << column << ": erro léxico: " << msg << "\n";

  string sourceLine = getLineContent(line);
  if (!sourceLine.empty()) {
    cout << "   " << line << " | " << sourceLine << "\n";
    int offset =
        3 + to_string(line).length() + 3 + (column > 0 ? column - 1 : 0);
    cout << string(offset, ' ') << "^\n";
  }

  errorCount++;
}

Token *Scanner::nextToken() {
  skipWhitespaceAndComments();
  if (isAtEnd()) {
    return makeToken(END_OF_FILE);
  }
  tokenLine = line;
  tokenColumn = column;

  char c = currentPeek();

  if (isalpha(c)) {
    return readIdentifier();
  }

  if (isdigit(c)) {
    return readNumber();
  }
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
  lexicalError(msg);
  advance();
  return nextToken();
}

bool Scanner::isAtEnd() { return pos >= input.length(); }
bool Scanner::matchCurrent(char expected) {
  if (isAtEnd())
    return false;
  if (currentPeek() != expected)
    return false;

  return true;
}
bool Scanner::matchNext(char expected) {
  if (isAtEnd())
    return false;
  if (nextPeek() != expected)
    return false;

  return true;
}

char Scanner::currentPeek() {
  if (isAtEnd())
    return '\0';
  return input[pos];
}
char Scanner::nextPeek() {
  if (pos + 1 >= input.length())
    return '\0';
  return input[pos + 1];
}
void Scanner::advance(int steps) {
  for (int i = 0; i < steps; i++) {
    if (pos < input.length()) {
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

Token *Scanner::makeToken(int name, int attribute, string lexeme) {
  return new Token(name, attribute, lexeme);
}
string Scanner::extractLexeme(int startPos, int endPos) {
  return input.substr(startPos, endPos - startPos);
}

void Scanner::skipWhitespaceAndComments() {
  while (true) {
    char c = currentPeek();

    if (isspace(c)) {
      advance();
    } else if (c == '/' && nextPeek() == '/') {
      advance(2);
      while (!matchCurrent('\n') && !isAtEnd()) {
        advance();
      }

      if (!isAtEnd()) {
        advance();
      }
    } else if (c == '/' && nextPeek() == '*') {
      advance(2);

      while (!(matchCurrent('*') && matchNext('/')) && !isAtEnd()) {
        advance();
      }

      if (!isAtEnd()) {
        advance(2);
      } else {
        lexicalError("comentário de bloco não fechado.");
      }
    } else {
      break;
    }
  }
}

Token *Scanner::readNumber() {
  int startPos = pos;

  while (isdigit(currentPeek())) {
    advance();
  }

  string lexeme = extractLexeme(startPos, pos);
  return makeToken(INTEGERCONSTANT, UNDEF, lexeme);
}
Token *Scanner::readCharLiteral() {
  int startPos = pos;
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
      lexicalError("tratamento de escape de caractere inválido.");
      return nullptr;
    }
  } else if (isprint(currentPeek()) && currentPeek() != '\'' &&
             currentPeek() != '\\') {
    advance();
    if (matchCurrent('\'')) {
      advance();
      string lexeme = extractLexeme(startPos + 1, pos - 1);
      return makeToken(CHARCONSTANT, CH, lexeme);
    }
  }

  lexicalError("literal de caractere mal formado.");
  return nullptr;
}
Token *Scanner::readStringLiteral() {
  int startPos = pos;
  advance();

  while (!isAtEnd() && currentPeek() != '"') {
    advance();
  }

  if (isAtEnd()) {
    lexicalError("string não fechada antes do fim do arquivo.");
    return nullptr;
  }

  advance();
  string lexeme = extractLexeme(startPos + 1, pos - 1);
  return makeToken(STRINGCONSTANT, UNDEF, lexeme);
}
Token *Scanner::readIdentifier() {
  int startPos = pos;

  while (isalpha(currentPeek()) || isdigit(currentPeek()) ||
         currentPeek() == '_') {
    advance();
  }

  string lexeme = extractLexeme(startPos, pos);

  return makeToken(ID, UNDEF, lexeme);
}
Token *Scanner::readOperator() {
  char c = currentPeek();
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
    lexicalError("operador '&' incompleto.");
    break;

  case '|':
    if (matchCurrent('|')) {
      advance();
      return makeToken(OR, UNDEF, "||");
    }
    lexicalError("operador '|' incompleto.");
    break;

  default:
    lexicalError("operador não reconhecido.");
    break;
  }

  return nullptr;
}
Token *Scanner::readSeparator() {
  char c = currentPeek();
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
    lexicalError("separador não reconhecido.");
    break;
  }

  return nullptr;
}