#ifndef __Scanner_h
#define __Scanner_h

#include "Token.h"
#include <fstream>

class Scanner {
private:
  string input;
  int pos;
  int line;
  int column;
  int tokenLine;
  int tokenColumn;
  int errorCount;

public:
  Scanner(string);

  int getLine();
  int getColumn();
  int getTokenLine();
  int getTokenColumn();
  int getErrorCount();

  string getLineContent(int lineNumber);

  Token *nextToken();

private:
  void lexicalError(string);
  char nextPeek();
  char currentPeek();

  bool isAtEnd();
  void advance(int steps = 1);

  bool matchCurrent(char expected);
  bool matchNext(char expected);

  void skipWhitespaceAndComments();

  string extractLexeme(int startPos, int endPos);

  Token *makeToken(int name, int attribute = UNDEF, string lexeme = "");
  Token *readNumber();

  Token *readIdentifier();
  Token *readCharLiteral();
  Token *readStringLiteral();

  Token *readOperator();
  Token *readSeparator();
};

#endif