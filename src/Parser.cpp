#include "../include/Parser.h"

Parser::Parser(Scanner *scan, Logger *log) {
  scanner = scan;
  logger = log;
  currentST = globalST = new SymbolTable();
  initSimbolTable();
}

// ==========================================
// Setup & Utilitários
// ==========================================
void Parser::initSimbolTable() {
  Token *t;

  t = new Token(VOID, "void");
  globalST->add(new STEntry(t, true));
  t = new Token(CHAR, "char");
  globalST->add(new STEntry(t, true));
  t = new Token(INT, "int");
  globalST->add(new STEntry(t, true));
  t = new Token(IF, "if");
  globalST->add(new STEntry(t, true));
  t = new Token(ELSE, "else");
  globalST->add(new STEntry(t, true));
  t = new Token(WHILE, "while");
  globalST->add(new STEntry(t, true));
  t = new Token(FOR, "for");
  globalST->add(new STEntry(t, true));
  t = new Token(RETURN, "return");
  globalST->add(new STEntry(t, true));
}

void Parser::run() {
  advanceToNextToken();
  try {
    parseGlobalDeclarationList();
    consumeExpectedToken(END_OF_FILE);
  } catch (const ParseError &e) {
  }

  int totalErrors = logger->getErrorCount();
  if (totalErrors > 0) {
    cout << "\033[1;31mCompilação falhou com " << totalErrors
         << " erro(s).\033[0m\n";
  } else {
    cout << "\033[1;32mCompilação encerrada com sucesso.\033[0m\n";
  }
}

void Parser::advanceToNextToken() { lToken = scanner->nextToken(); }

void Parser::consumeExpectedToken(int token) {
  if (checkSingleToken(token)) {
    advanceToNextToken();
  } else {
    string expected = tokenToString(token);
    throwError("esperado '" + expected + "'");
  }
}

void Parser::parseError(string str) {
  int len = (lToken && !lToken->lexeme.empty()) ? lToken->lexeme.length() : 1;
  logger->printDiagnostic(scanner->getLine(), scanner->getTokenColumn(), str,
                          len);
}

void Parser::throwError(string str) {
  parseError(str);
  throw ParseError(str);
}

void Parser::synchronize() {
  while (lToken->name != END_OF_FILE) {
    if (checkTokens(ID, LBRACE, RBRACE)) {
      return;
    }
    if (lToken->name == SEMICOLON) {
      advanceToNextToken();
      return;
    }
    advanceToNextToken();
  }
}

bool Parser::isStartOfExpression() {
  return (checkTokens(MINUS, NOT, ID, LPAREN, INTEGERCONSTANT, CHARCONSTANT,
                      STRINGCONSTANT));
}

bool Parser::isStartOfStatement() {
  return (checkTokens(ID, LBRACE, SEMICOLON));
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

  Token *idToken = lToken;
  consumeExpectedToken(ID);

  if (checkTokens(LPAREN)) {
    // É uma função
    if (!currentST->add(new STEntry(idToken, false)))
      parseError("função já declarada neste escopo: " + idToken->lexeme);
    createNewContext();
    consumeExpectedToken(LPAREN);
    parseParamTypes();
    consumeExpectedToken(RPAREN);
    consumeExpectedToken(LBRACE);
    parseLocalDeclarations();
    parseStatementList();
    consumeExpectedToken(RBRACE);
    backToPreviousContext();
  } else {
    // É uma variável global
    if (isVoid) {
      throwError("variáveis não podem ser do tipo 'void'");
    }
    if (!currentST->add(new STEntry(idToken, false)))
      parseError("variável global já declarada neste escopo: " +
                 idToken->lexeme);
    parseArraySize();
    parseMoreVariables();
    consumeExpectedToken(SEMICOLON);
  }
}

void Parser::parseParamTypes() {
  if (checkTokens(RPAREN))
    return;

  if (checkLexemes("void"))
    advanceToNextToken();
  else if (checkLexemes("char", "int")) {
    advanceToNextToken();
    Token *idToken = lToken;
    consumeExpectedToken(ID);
    if (!currentST->add(new STEntry(idToken, false)))
      parseError("parâmetro já declarado neste escopo: " + idToken->lexeme);
    parseArrayBrackets();
    parseMoreParameters();
  } else
    throwError("tipo de retorno inválido (esperado 'int', 'char' ou 'void')");
}

void Parser::parseMoreParameters() {
  if (checkTokens(COMMA)) {
    advanceToNextToken();
    parseType();
    Token *idToken = lToken;
    consumeExpectedToken(ID);
    if (!currentST->add(new STEntry(idToken, false)))
      parseError("parâmetro já declarado neste escopo: " + idToken->lexeme);
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
      consumeExpectedToken(SEMICOLON);
    } catch (const ParseError &e) {
      synchronize();
    }
    parseLocalDeclarations();
  }
}

void Parser::parseVariableDeclaration() {
  Token *idToken = lToken;
  consumeExpectedToken(ID);
  if (!currentST->add(new STEntry(idToken, false)))
    parseError("variável já declarada neste escopo: " + idToken->lexeme);
  parseArraySize();
}

void Parser::parseMoreVariables() {
  if (checkTokens(COMMA)) {
    advanceToNextToken();
    parseVariableDeclaration();
    parseMoreVariables();
  }
}

void Parser::parseArraySize() {
  if (checkTokens(LBRACKET)) {
    advanceToNextToken();
    consumeExpectedToken(INTEGERCONSTANT);
    consumeExpectedToken(RBRACKET);
  }
}

void Parser::parseArrayBrackets() {
  if (checkTokens(LBRACKET)) {
    advanceToNextToken();
    consumeExpectedToken(RBRACKET);
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
    parseError("declarações de variáveis devem vir antes das "
               "instruções/comandos do bloco");
    parseLocalDeclarations();
  } else if (checkTokens(ID)) {
    Token *idToken = lToken;
    if (!currentST->get(idToken->lexeme))
      parseError("variável ou função não declarada: " + idToken->lexeme);
    advanceToNextToken();
    parseAssignmentOrCall();
  } else if (checkTokens(LBRACE)) {
    parseBlock();
  } else if (checkTokens(SEMICOLON)) {
    advanceToNextToken();
  } else {
    throwError("início de instrução inválido");
  }
}

void Parser::parseIfStatement() {
  advanceToNextToken();
  consumeExpectedToken(LPAREN);
  createNewContext();
  parseExpression();
  consumeExpectedToken(RPAREN);
  parseStatement();
  parseOptionalElse();
  backToPreviousContext();
}

void Parser::parseWhileStatement() {
  advanceToNextToken();
  consumeExpectedToken(LPAREN);
  createNewContext();
  parseExpression();
  consumeExpectedToken(RPAREN);
  parseStatement();
  backToPreviousContext();
}

void Parser::parseForStatement() {
  advanceToNextToken();
  consumeExpectedToken(LPAREN);
  createNewContext();
  parseOptionalAssignment();
  consumeExpectedToken(SEMICOLON);
  parseOptionalExpression();
  consumeExpectedToken(SEMICOLON);
  parseOptionalAssignment();
  consumeExpectedToken(RPAREN);
  parseStatement();
  backToPreviousContext();
}

void Parser::parseReturnStatement() {
  advanceToNextToken();
  parseOptionalExpression();
  consumeExpectedToken(SEMICOLON);
}

void Parser::parseBlock() {
  createNewContext();
  advanceToNextToken();
  parseStatementList();
  consumeExpectedToken(RBRACE);
  backToPreviousContext();
}

void Parser::parseOptionalElse() {
  if (checkLexemes("else")) {
    advanceToNextToken();
    parseStatement();
  }
}

void Parser::parseOptionalAssignment() {
  if (checkTokens(ID)) {
    parseAssignment();
  }
}

void Parser::parseAssignment() {
  Token *idToken = lToken;
  consumeExpectedToken(ID);
  if (!currentST->get(idToken->lexeme))
    parseError("variável ou função não declarada: " + idToken->lexeme);
  parseArrayAccess();
  consumeExpectedToken(ASSIGN);
  parseExpression();
}

void Parser::parseAssignmentOrCall() {
  if (checkTokens(LPAREN)) {
    advanceToNextToken();
    parseArgumentList();
    consumeExpectedToken(RPAREN);
    consumeExpectedToken(SEMICOLON);
  } else if (checkTokens(LBRACKET, ASSIGN)) {
    parseArrayAccess();
    consumeExpectedToken(ASSIGN);
    parseExpression();
    consumeExpectedToken(SEMICOLON);
  } else
    throwError("esperada atribuição ('=') ou chamada de função ('(') após o "
               "identificador");
}

// ==========================================
// Expressions
// ==========================================
void Parser::parseExpression() {
  if (checkTokens(MINUS, NOT)) {
    advanceToNextToken();
    parseExpression();
    parseExpressionTail();
  } else if (checkTokens(LPAREN)) {
    advanceToNextToken();
    parseExpression();
    consumeExpectedToken(RPAREN);
    parseExpressionTail();
  } else if (checkTokens(ID)) {
    Token *idToken = lToken;
    if (!currentST->get(idToken->lexeme))
      parseError("variável ou função não declarada: " + idToken->lexeme);
    advanceToNextToken();
    parseIdentifierSuffix();
    parseExpressionTail();
  } else if (checkTokens(INTEGERCONSTANT, CHARCONSTANT, STRINGCONSTANT)) {
    advanceToNextToken();
    parseExpressionTail();
  } else {
    throwError("expressão inválida ou incompleta");
  }
}

void Parser::parseExpressionTail() {
  if (checkTokens(PLUS, MINUS, TIMES, DIVIDED)) {
    parseBinaryOperator();
    parseExpression();
    parseExpressionTail();
  } else if (checkTokens(EQUIVALENT, DIFF, LE, GE, LT, GT)) {
    parseRelationalOperator();
    parseExpression();
    parseExpressionTail();
  } else if (checkTokens(AND, OR)) {
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
  if (checkTokens(LPAREN)) {
    advanceToNextToken();
    parseArgumentList();
    consumeExpectedToken(RPAREN);
  } else if (checkTokens(LBRACKET)) {
    advanceToNextToken();
    parseExpression();
    consumeExpectedToken(RBRACKET);
  }
}

void Parser::parseArrayAccess() {
  if (checkTokens(LBRACKET)) {
    advanceToNextToken();
    parseExpression();
    consumeExpectedToken(RBRACKET);
  }
}

void Parser::parseArgumentList() {
  if (isStartOfExpression()) {
    parseExpression();
    parseMoreExpressions();
  }
}

void Parser::parseMoreExpressions() {
  if (checkTokens(COMMA)) {
    advanceToNextToken();
    parseExpression();
    parseMoreExpressions();
  }
}

void Parser::parseBinaryOperator() {
  if (checkTokens(PLUS, MINUS, TIMES, DIVIDED)) {
    advanceToNextToken();
  } else {
    throwError("operador aritmético esperado ('+', '-', '*', '/')");
  }
}

void Parser::parseRelationalOperator() {
  if (checkTokens(EQUIVALENT, DIFF, LE, GE, LT, GT)) {
    advanceToNextToken();
  } else {
    throwError(
        "operador relacional esperado ('==', '!=', '<', '>', '<=', '>=')");
  }
}

void Parser::parseLogicalOperator() {
  if (checkTokens(AND, OR)) {
    advanceToNextToken();
  } else {
    throwError("operador lógico esperado ('&&', '||')");
  }
}