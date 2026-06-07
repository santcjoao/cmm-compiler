#include "../include/Parser.h"

using namespace std;

Parser::Parser(Scanner *scan, Logger *log) {
  scanner = scan;
  logger = log;

  SymbolTable *table = new SymbolTable();
  globalST = currentST = table;
  ownedTables.push_back(table);

  initSimbolTable();
}

// ==========================================
// Setup & Utilitários
// ==========================================
Parser::~Parser() {
  for (size_t i = 0; i < ownedTables.size(); i++) {
    delete ownedTables[i];
  }
}

void Parser::initSimbolTable() {
  globalST->add(STEntry(Token(TokenType::VOID, "void"), true));
  globalST->add(STEntry(Token(TokenType::CHAR, "char"), true));
  globalST->add(STEntry(Token(TokenType::INT, "int"), true));
  globalST->add(STEntry(Token(TokenType::IF, "if"), true));
  globalST->add(STEntry(Token(TokenType::ELSE, "else"), true));
  globalST->add(STEntry(Token(TokenType::WHILE, "while"), true));
  globalST->add(STEntry(Token(TokenType::FOR, "for"), true));
  globalST->add(STEntry(Token(TokenType::RETURN, "return"), true));
}

void Parser::run() {
  advanceToNextToken();
  try {
    parseGlobalDeclarationList();
    consumeExpectedToken(TokenType::END_OF_FILE);
  } catch (const ParseError &e) {
  }

  logger->printSummary();
}

void Parser::advanceToNextToken() { lToken = scanner->nextToken(); }

void Parser::consumeExpectedToken(TokenType token) {
  if (checkSingleToken(token)) {
    advanceToNextToken();
  } else {
    string expected = tokenToString(token);
    throwError("esperado '" + expected + "'");
  }
}

void Parser::parseError(const string &str) {
  int len = !lToken.lexeme.empty() ? (int)lToken.lexeme.length() : 1;
  logger->error(scanner->getLine(), scanner->getTokenColumn(), str, len);
}

void Parser::throwError(const string &str) {
  parseError(str);
  throw ParseError(str);
}

void Parser::synchronize() {
  while (lToken.name != TokenType::END_OF_FILE) {
    if (checkTokens(TokenType::ID, TokenType::LBRACE, TokenType::RBRACE)) {
      return;
    }
    if (lToken.name == TokenType::SEMICOLON) {
      advanceToNextToken();
      return;
    }
    advanceToNextToken();
  }
}

bool Parser::isStartOfExpression() {
  return (checkTokens(TokenType::MINUS, TokenType::NOT, TokenType::ID,
                      TokenType::LPAREN, TokenType::INTEGERCONSTANT,
                      TokenType::CHARCONSTANT, TokenType::STRINGCONSTANT));
}

bool Parser::isStartOfStatement() {
  return (checkTokens(TokenType::ID, TokenType::LBRACE, TokenType::SEMICOLON));
}

// ==========================================
// Declarações
// ==========================================
void Parser::parseGlobalDeclarationList() {
  if (checkLexemes("void", "char", "int")) {
    try {
      parseGlobalDeclaration();
    } catch (const ParseError &e) {
      synchronize();
    }
    parseGlobalDeclarationList();
  }
}

void Parser::parseGlobalDeclaration() {
  bool isVoid = false;
  if (checkLexemes("void")) {
    isVoid = true;
    advanceToNextToken();
  } else {
    parseType();
  }

  Token idToken = lToken;
  consumeExpectedToken(TokenType::ID);

  if (checkTokens(TokenType::LPAREN)) {
    // É uma função
    if (!currentST->add(STEntry(idToken)))
      parseError("função já declarada neste escopo: " + idToken.lexeme);
    createNewContext();
    consumeExpectedToken(TokenType::LPAREN);
    parseParamTypes();
    consumeExpectedToken(TokenType::RPAREN);
    consumeExpectedToken(TokenType::LBRACE);
    parseLocalDeclarations();
    parseStatementList();
    consumeExpectedToken(TokenType::RBRACE);
    backToPreviousContext();
  } else {
    // É uma variável global
    if (isVoid) {
      throwError("variáveis não podem ser do tipo 'void'");
    }
    if (!currentST->add(STEntry(idToken)))
      parseError("variável global já declarada neste escopo: " +
                 idToken.lexeme);
    parseArraySize();
    parseMoreVariables();
    consumeExpectedToken(TokenType::SEMICOLON);
  }
}

void Parser::parseParamTypes() {
  if (checkTokens(TokenType::RPAREN))
    return;

  if (checkLexemes("void"))
    advanceToNextToken();
  else if (checkLexemes("char", "int")) {
    advanceToNextToken();
    Token idToken = lToken;
    consumeExpectedToken(TokenType::ID);
    if (!currentST->add(STEntry(idToken)))
      parseError("parâmetro já declarado neste escopo: " + idToken.lexeme);
    parseArrayBrackets();
    parseMoreParameters();
  } else
    throwError("tipo de retorno inválido (esperado 'int', 'char' ou 'void')");
}

void Parser::parseMoreParameters() {
  if (checkTokens(TokenType::COMMA)) {
    advanceToNextToken();
    parseType();
    Token idToken = lToken;
    consumeExpectedToken(TokenType::ID);
    if (!currentST->add(STEntry(idToken)))
      parseError("parâmetro já declarado neste escopo: " + idToken.lexeme);
    parseArrayBrackets();
    parseMoreParameters();
  }
}

void Parser::parseType() {
  if (checkLexemes("char", "int"))
    advanceToNextToken();
  else
    throwError("tipo de dado inválido (esperado 'int' ou 'char')");
}

void Parser::parseLocalDeclarations() {
  if (checkLexemes("char", "int")) {
    try {
      parseType();
      parseVariableDeclaration();
      parseMoreVariables();
      consumeExpectedToken(TokenType::SEMICOLON);
    } catch (const ParseError &e) {
      synchronize();
    }
    parseLocalDeclarations();
  }
}

void Parser::parseVariableDeclaration() {
  Token idToken = lToken;
  consumeExpectedToken(TokenType::ID);
  if (!currentST->add(STEntry(idToken)))
    parseError("variável já declarada neste escopo: " + idToken.lexeme);
  parseArraySize();
}

void Parser::parseMoreVariables() {
  if (checkTokens(TokenType::COMMA)) {
    advanceToNextToken();
    parseVariableDeclaration();
    parseMoreVariables();
  }
}

void Parser::parseArraySize() {
  if (checkTokens(TokenType::LBRACKET)) {
    advanceToNextToken();
    consumeExpectedToken(TokenType::INTEGERCONSTANT);
    consumeExpectedToken(TokenType::RBRACKET);
  }
}

void Parser::parseArrayBrackets() {
  if (checkTokens(TokenType::LBRACKET)) {
    advanceToNextToken();
    consumeExpectedToken(TokenType::RBRACKET);
  }
}

// ==========================================
// Statements
// ==========================================
void Parser::parseStatementList() {
  if (isStartOfStatement()) {
    try {
      parseStatement();
    } catch (const ParseError &e) {
      synchronize();
    }
    parseStatementList();
  }
}

void Parser::parseStatement() {
  if (checkLexemes("if")) {
    parseIfStatement();
  } else if (checkLexemes("while")) {
    parseWhileStatement();
  } else if (checkLexemes("for")) {
    parseForStatement();
  } else if (checkLexemes("return")) {
    parseReturnStatement();
  } else if (checkLexemes("int", "char")) {
    logger->warning(scanner->getLine(), scanner->getTokenColumn(),
                    "declarações de variáveis devem vir antes das "
                    "instruções/comandos do bloco");
    parseLocalDeclarations();
  } else if (checkTokens(TokenType::ID)) {
    Token idToken = lToken;
    if (!currentST->get(idToken.lexeme))
      parseError("variável ou função não declarada: " + idToken.lexeme);
    advanceToNextToken();
    parseAssignmentOrCall();
  } else if (checkTokens(TokenType::LBRACE)) {
    parseBlock();
  } else if (checkTokens(TokenType::SEMICOLON)) {
    advanceToNextToken();
  } else {
    throwError("início de instrução inválido");
  }
}

void Parser::parseIfStatement() {
  advanceToNextToken();
  consumeExpectedToken(TokenType::LPAREN);
  createNewContext();
  parseExpression();
  consumeExpectedToken(TokenType::RPAREN);
  parseStatement();
  parseOptionalElse();
  backToPreviousContext();
}

void Parser::parseWhileStatement() {
  advanceToNextToken();
  consumeExpectedToken(TokenType::LPAREN);
  createNewContext();
  parseExpression();
  consumeExpectedToken(TokenType::RPAREN);
  parseStatement();
  backToPreviousContext();
}

void Parser::parseForStatement() {
  advanceToNextToken();
  consumeExpectedToken(TokenType::LPAREN);
  createNewContext();
  parseOptionalAssignment();
  consumeExpectedToken(TokenType::SEMICOLON);
  parseOptionalExpression();
  consumeExpectedToken(TokenType::SEMICOLON);
  parseOptionalAssignment();
  consumeExpectedToken(TokenType::RPAREN);
  parseStatement();
  backToPreviousContext();
}

void Parser::parseReturnStatement() {
  advanceToNextToken();
  parseOptionalExpression();
  consumeExpectedToken(TokenType::SEMICOLON);
}

void Parser::parseBlock() {
  createNewContext();
  advanceToNextToken();
  parseStatementList();
  consumeExpectedToken(TokenType::RBRACE);
  backToPreviousContext();
}

void Parser::parseOptionalElse() {
  if (checkLexemes("else")) {
    advanceToNextToken();
    parseStatement();
  }
}

void Parser::parseOptionalAssignment() {
  if (checkTokens(TokenType::ID)) {
    parseAssignment();
  }
}

void Parser::parseAssignment() {
  Token idToken = lToken;
  consumeExpectedToken(TokenType::ID);
  if (!currentST->get(idToken.lexeme))
    parseError("variável ou função não declarada: " + idToken.lexeme);
  parseArrayAccess();
  consumeExpectedToken(TokenType::ASSIGN);
  parseExpression();
}

void Parser::parseAssignmentOrCall() {
  if (checkTokens(TokenType::LPAREN)) {
    advanceToNextToken();
    parseArgumentList();
    consumeExpectedToken(TokenType::RPAREN);
    consumeExpectedToken(TokenType::SEMICOLON);
  } else if (checkTokens(TokenType::LBRACKET, TokenType::ASSIGN)) {
    parseArrayAccess();
    consumeExpectedToken(TokenType::ASSIGN);
    parseExpression();
    consumeExpectedToken(TokenType::SEMICOLON);
  } else
    throwError("esperada atribuição ('=') ou chamada de função ('(') após o "
               "identificador");
}

// ==========================================
// Expressions
// ==========================================
void Parser::parseExpression() {
  if (checkTokens(TokenType::MINUS, TokenType::NOT)) {
    advanceToNextToken();
    parseExpression();
    parseExpressionTail();
  } else if (checkTokens(TokenType::LPAREN)) {
    advanceToNextToken();
    parseExpression();
    consumeExpectedToken(TokenType::RPAREN);
    parseExpressionTail();
  } else if (checkTokens(TokenType::ID)) {
    Token idToken = lToken;
    if (!currentST->get(idToken.lexeme))
      parseError("variável ou função não declarada: " + idToken.lexeme);
    advanceToNextToken();
    parseIdentifierSuffix();
    parseExpressionTail();
  } else if (checkTokens(TokenType::INTEGERCONSTANT, TokenType::CHARCONSTANT,
                         TokenType::STRINGCONSTANT)) {
    advanceToNextToken();
    parseExpressionTail();
  } else {
    throwError("expressão inválida ou incompleta");
  }
}

void Parser::parseExpressionTail() {
  if (checkTokens(TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
                  TokenType::DIVIDED)) {
    parseBinaryOperator();
    parseExpression();
    parseExpressionTail();
  } else if (checkTokens(TokenType::EQUIVALENT, TokenType::DIFF, TokenType::LE,
                         TokenType::GE, TokenType::LT, TokenType::GT)) {
    parseRelationalOperator();
    parseExpression();
    parseExpressionTail();
  } else if (checkTokens(TokenType::AND, TokenType::OR)) {
    parseLogicalOperator();
    parseExpression();
    parseExpressionTail();
  }
}

void Parser::parseOptionalExpression() {
  if (isStartOfExpression()) {
    parseExpression();
  }
}

void Parser::parseIdentifierSuffix() {
  if (checkTokens(TokenType::LPAREN)) {
    advanceToNextToken();
    parseArgumentList();
    consumeExpectedToken(TokenType::RPAREN);
  } else if (checkTokens(TokenType::LBRACKET)) {
    advanceToNextToken();
    parseExpression();
    consumeExpectedToken(TokenType::RBRACKET);
  }
}

void Parser::parseArrayAccess() {
  if (checkTokens(TokenType::LBRACKET)) {
    advanceToNextToken();
    parseExpression();
    consumeExpectedToken(TokenType::RBRACKET);
  }
}

void Parser::parseArgumentList() {
  if (isStartOfExpression()) {
    parseExpression();
    parseMoreExpressions();
  }
}

void Parser::parseMoreExpressions() {
  if (checkTokens(TokenType::COMMA)) {
    advanceToNextToken();
    parseExpression();
    parseMoreExpressions();
  }
}

void Parser::parseBinaryOperator() {
  if (checkTokens(TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
                  TokenType::DIVIDED)) {
    advanceToNextToken();
  } else {
    throwError("operador aritmético esperado ('+', '-', '*', '/')");
  }
}

void Parser::parseRelationalOperator() {
  if (checkTokens(TokenType::EQUIVALENT, TokenType::DIFF, TokenType::LE,
                  TokenType::GE, TokenType::LT, TokenType::GT)) {
    advanceToNextToken();
  } else {
    throwError(
        "operador relacional esperado ('==', '!=', '<', '>', '<=', '>=')");
  }
}

void Parser::parseLogicalOperator() {
  if (checkTokens(TokenType::AND, TokenType::OR)) {
    advanceToNextToken();
  } else {
    throwError("operador lógico esperado ('&&', '||')");
  }
}