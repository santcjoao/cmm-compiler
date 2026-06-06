#ifndef __Scanner_h
#define __Scanner_h

#include "Logger.h"
#include "Token.h"

class Scanner {
public:
  Scanner(Logger *logger);
  Token *nextToken();

private:
  string input;
  Logger *logger;

  int pos;
  int line;
  int column;
  int tokenColumn;

  // ==========================================
  // Setup & Utilitários
  // ==========================================
public:
  int getLine();
  int getTokenColumn();

private:
  void lexicalError(int errLine, int errCol, string msg, int len = 1);
  void advance(int steps = 1);
  void skipWhitespaceAndComments();

  inline bool isAtEnd() { return pos >= (int)input.length(); }
  inline char currentPeek() { return isAtEnd() ? '\0' : input[pos]; }
  inline char nextPeek() {
    return pos + 1 >= (int)input.length() ? '\0' : input[pos + 1];
  }
  inline bool matchCurrent(char expected) {
    return !isAtEnd() && input[pos] == expected;
  }
  inline bool matchNext(char expected) {
    return pos + 1 < (int)input.length() && input[pos + 1] == expected;
  }
  inline Token *makeToken(int name, int attribute = UNDEF, string lexeme = "") {
    return new Token(name, attribute, lexeme);
  }
  inline string extractLexeme(int startPos, int endPos) {
    return input.substr(startPos, endPos - startPos);
  }

  // ==========================================
  // Leitura de Tokens
  // ==========================================
  Token *readNumber();
  Token *readIdentifier();
  Token *readCharLiteral();
  Token *readStringLiteral();
  Token *readOperator();
  Token *readSeparator();
};

#endif