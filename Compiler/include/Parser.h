#ifndef __Parser_h
#define __Parser_h

#include "Scanner.h"

class Parser {
private:
  Scanner *scanner;
  Token *lToken;

  void advanceToNextToken();
  void consumeExpectedToken(int);
  void error(string);

  bool isStartOfStatement();
  bool isStartOfExpression();

  inline bool checkSingleToken(int Token) {
    return (lToken->name == Token || lToken->attribute == Token);
  }

  template <typename... Tokens> bool checkTokens(Tokens... tokens) {
    return (checkSingleToken(tokens) || ...);
  }

  inline bool checkSingleKeyword(int kw) {
    return (lToken->name == ID && lToken->attribute == kw);
  }

  template <typename... Keywords> bool checkKeywords(Keywords... kws) {
    return (checkSingleKeyword(kws) || ...);
  }

public:
  Parser(string);
  void run();
  void program();

  void parseFunctionList();
  void parseFunction();
  void parseParamTypes();
  void parseMoreParameters();
  void parseType();
  void parseLocalDeclarations();
  void parseVariableDeclaration();
  void parseMoreVariables();
  void parseArraySize();
  void parseArrayBrackets();

  void parseStatement();
  void parseStatementList();
  void parseOptionalElse();
  void parseOptionalAssignment();
  void parseAssignment();
  void parseAssignmentOrCall();

  void parseExpression();
  void parseExpressionTail();
  void parseOptionalExpression();
  void parseIdentifierSuffix();
  void parseArrayAccess();
  void parseArgumentList();
  void parseMoreExpressions();

  void parseBinaryOperator();
  void parseRelationalOperator();
  void parseLogicalOperator();
};

#endif