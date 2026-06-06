#ifndef __Scanner_h
#define __Scanner_h

#include "Logger.h"
#include "Token.h"

#include <string>

class SourceFile;

class Scanner {
public:
  Scanner(const SourceFile &sourceFile, Logger *logger);
  Token nextToken();

private:
  std::string input;
  Logger *logger;

  int pos;
  int line;
  int column;
  int tokenColumn;

  // ==========================================
  // Setup & Utilitários
  // ==========================================
public:
  int getLine() const;
  int getTokenColumn() const;

private:
  void lexicalError(int errLine, int errCol, const std::string &msg,
                    int len = 1);
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
  inline Token makeToken(TokenType name, TokenType attribute = TokenType::UNDEF,
                         const std::string &lexeme = "") {
    return Token(name, attribute, lexeme);
  }
  inline std::string extractLexeme(int startPos, int endPos) {
    return input.substr(startPos, endPos - startPos);
  }

  // ==========================================
  // Leitura de Tokens
  // ==========================================
  Token readNumber();
  Token readIdentifier();
  bool readCharLiteral(Token &out);
  bool readStringLiteral(Token &out);
  bool readOperator(Token &out);
  Token readSeparator();
};

#endif