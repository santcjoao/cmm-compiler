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

public:
  Scanner(string);

  int getLine();
  int getColumn();
  int getTokenLine();
  int getTokenColumn();

  void lexicalError(string);
  char nextPeek();
  char currentPeek();

  bool isAtEnd();
  void advance(int steps = 1);

  bool matchCurrent(char expected);
  bool matchNext(char expected);

  void skipWhitespaceAndComments();

  string getLineContent(int lineNumber);
  string extractLexeme(int startPos, int endPos);

  Token *nextToken();
  Token *readNumber();

  Token *readCharLiteral();
  Token *readStringLiteral();
  Token *readIdentifierOrKeyword();

  Token *readOperator();
  Token *readSeparator();
  Token *makeToken(int name, int attribute = UNDEF, string lexeme = "");
};
