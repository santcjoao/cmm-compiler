#ifndef __Parser_h
#define __Parser_h

#include "Logger.h"
#include "Scanner.h"
#include "SymbolTable.h"

#include <stdexcept>
#include <string>
#include <vector>

class ParseError : public std::runtime_error {
public:
  ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

class Parser {
public:
  Parser(Scanner *scanner, Logger *logger);
  ~Parser();
  void run();
  SymbolTable *getSymbolTable() { return globalST; }

private:
  // --- Atributos ---
  Scanner *scanner;
  Logger *logger;
  Token lToken;

  std::vector<SymbolTable *> ownedTables;
  SymbolTable *globalST;
  SymbolTable *currentST;

  // ==========================================
  // Setup & Utilitários
  // ==========================================
  void initSimbolTable();
  void advanceToNextToken();
  void consumeExpectedToken(TokenType token);

  void parseError(const std::string &msg);
  void throwError(const std::string &msg);
  void synchronize();

  inline bool checkSingleToken(TokenType token) {
    return (lToken.name == token);
  }
  template <typename... Tokens> bool checkTokens(Tokens... tokens) {
    return (checkSingleToken(tokens) || ...);
  }
  inline bool checkLexeme(const std::string &str) {
    return (lToken.name == TokenType::ID && lToken.lexeme == str);
  }
  template <typename... Strings> bool checkLexemes(Strings... strs) {
    return (checkLexeme(strs) || ...);
  }

  void createNewContext() {
    SymbolTable *table = new SymbolTable(currentST);
    currentST = table;
    ownedTables.push_back(table);
  }
  void backToPreviousContext() {
    if (currentST->getParent())
      currentST = currentST->getParent();
  }

  bool isStartOfStatement();
  bool isStartOfExpression();

  // ==========================================
  // Declarações
  // ==========================================
  void parseGlobalDeclarationList();
  void parseGlobalDeclaration();
  void parseParamTypes();
  void parseMoreParameters();
  void parseType();
  void parseLocalDeclarations();
  void parseVariableDeclaration();
  void parseMoreVariables();
  void parseArraySize();
  void parseArrayBrackets();

  // ==========================================
  // Statements
  // ==========================================
  void parseStatementList();
  void parseStatement();
  void parseIfStatement();
  void parseWhileStatement();
  void parseForStatement();
  void parseReturnStatement();
  void parseBlock();
  void parseOptionalElse();
  void parseOptionalAssignment();
  void parseAssignment();
  void parseAssignmentOrCall();

  // ==========================================
  // Expressions
  // ==========================================
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