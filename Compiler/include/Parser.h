#ifndef __Parser_h
#define __Parser_h

#include "Scanner.h"
#include "symboltable.h"
#include <stdexcept>

class ParseError : public std::runtime_error {
public:
  ParseError(const string &msg) : std::runtime_error(msg) {}
};

class Parser {
public:
  Parser(string);
  void run();

private:
  Scanner *scanner;
  Token *lToken;
  SymbolTable* globalST;
  SymbolTable* currentST;
  int errorCount;

  void initSimbolTable();

  inline bool checkSingleToken(int Token) { return (lToken->name == Token); }

  template <typename... Tokens> bool checkTokens(Tokens... tokens) {
    return (checkSingleToken(tokens) || ...);
  }

  inline bool checkLexeme(string str) {
    return (lToken->name == ID && lToken->lexeme == str);
  }

  template <typename... Strings> bool checkLexemes(Strings... strs) {
    return (checkLexeme(strs) || ...);
  }

  void advanceToNextToken();
  void consumeExpectedToken(int);

  void reportError(string);
  void error(string);
  void synchronize();

  bool isStartOfStatement();
  bool isStartOfExpression();

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