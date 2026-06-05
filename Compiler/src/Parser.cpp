#include "../include/Parser.h"

Parser::Parser(string input) {
  currentST = globalST = new SymbolTable();
  initSimbolTable();
  scanner = new Scanner(input, globalST);
  errorCount = 0;
}

void Parser::initSimbolTable() {
  Token* t;

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
    parseFunctionList();
    consumeExpectedToken(END_OF_FILE);
  } catch (const ParseError &e) {
  }

  int totalErrors = errorCount + scanner->getErrorCount();
  if (totalErrors > 0) {
    cout << "Compilação falhou com " << totalErrors << " erro(s).\n";
    exit(EXIT_FAILURE);
  }

  cout << "Compilação encerrada.\n";
}

void Parser::advanceToNextToken() { lToken = scanner->nextToken(); }

void Parser::consumeExpectedToken(int Token) {
  if (checkSingleToken(Token)) {
    advanceToNextToken();
  } else {
    string expected = tokenToString(Token);
    error("esperado '" + expected + "'");
  }
}

bool Parser::isStartOfExpression() {
  return (checkTokens(MINUS, NOT, ID, LPAREN, INTEGERCONSTANT, CHARCONSTANT,
                      STRINGCONSTANT));
}

bool Parser::isStartOfStatement() {
  return (checkTokens(ID, LBRACE, SEMICOLON));
}

void Parser::reportError(string str) {
  int line = scanner->getTokenLine();
  int col = scanner->getTokenColumn();

  cout << "arquivo.cmm:" << line << ":" << col << ": erro sintático: " << str << "\n";

  string sourceLine = scanner->getLineContent(line);
  if (!sourceLine.empty()) {
    cout << "   " << line << " | " << sourceLine << "\n";
    int offset = 3 + to_string(line).length() + 3 + (col > 0 ? col - 1 : 0);
    cout << string(offset, ' ') << "^\n";
  }

  errorCount++;
}

void Parser::error(string str) {
  reportError(str);
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

void Parser::parseFunctionList() {
  if (checkLexemes("void", "char", "int")) {
    try {
      parseFunction();
    } catch (const ParseError &e) {
      synchronize();
    }
    parseFunctionList();
  }
}

void Parser::parseFunction() {
  if (checkLexemes("void"))
    advanceToNextToken();
  else
    parseType();
  Token* idToken = lToken;
  consumeExpectedToken(ID);
  currentST->add(new STEntry(idToken, false));
  createNewContext();
  consumeExpectedToken(LPAREN);
  parseParamTypes();
  consumeExpectedToken(RPAREN);
  consumeExpectedToken(LBRACE);
  parseLocalDeclarations();
  parseStatementList();
  consumeExpectedToken(RBRACE);
  backToPreviousContext();
}

void Parser::parseParamTypes() {
  if (checkLexemes("void"))
    advanceToNextToken();
  else if (checkLexemes("char", "int")) {
    advanceToNextToken();
    Token* idToken = lToken;
    consumeExpectedToken(ID);
    currentST->add(new STEntry(idToken, false));
    parseArrayBrackets();
    parseMoreParameters();
  } else
    error("tipo de retorno inválido (esperado 'int', 'char' ou 'void')");
}

void Parser::parseMoreParameters() {
  if (checkTokens(COMMA)) {
    advanceToNextToken();
    parseType();
    Token* idToken = lToken;
    consumeExpectedToken(ID);
    currentST->add(new STEntry(idToken, false));
    parseArrayBrackets();
    parseMoreParameters();
  }
}

void Parser::parseType() {
  if (checkLexemes("char", "int"))
    advanceToNextToken();
  else
    error("tipo de dado inválido (esperado 'int' ou 'char')");
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
  Token* idToken = lToken;
  consumeExpectedToken(ID);
  currentST->add(new STEntry(idToken, false));
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

void Parser::parseStatement() {
  if (checkLexemes("if")) {
    advanceToNextToken();
    consumeExpectedToken(LPAREN);
    createNewContext();
    parseExpression();
    consumeExpectedToken(RPAREN);
    parseStatement();
    parseOptionalElse();
    backToPreviousContext();
  } else if (checkLexemes("while")) {
    advanceToNextToken();
    consumeExpectedToken(LPAREN);
    createNewContext();
    parseExpression();
    consumeExpectedToken(RPAREN);
    parseStatement();
    backToPreviousContext();
  } else if (checkLexemes("for")) {
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
  } else if (checkLexemes("return")) {
    advanceToNextToken();
    parseOptionalExpression();
    consumeExpectedToken(SEMICOLON);
  } else if (checkLexemes("int", "char")) {
    reportError("declarações de variáveis devem vir antes das "
                "instruções/comandos do bloco");
    parseLocalDeclarations();
  } else if (checkTokens(ID)) {
    Token* idToken = lToken;
    currentST->add(new STEntry(idToken, false));
    advanceToNextToken();
    parseAssignmentOrCall();
  } else if (checkTokens(LBRACE)) {
    createNewContext();
    advanceToNextToken();
    parseStatementList();
    consumeExpectedToken(RBRACE);
    backToPreviousContext();
  } else if (checkTokens(SEMICOLON))
    advanceToNextToken();
  else
    error("início de instrução inválido");
}



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
  Token* idToken = lToken;
  consumeExpectedToken(ID);
  currentST->add(new STEntry(idToken, false));
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
    error("esperada atribuição ('=') ou chamada de função ('(') após o "
          "identificador");
}

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
    Token* idToken = lToken;
    currentST->add(new STEntry(idToken, false));
    advanceToNextToken();
    parseIdentifierSuffix();
    parseExpressionTail();
  } else if (checkTokens(INTEGERCONSTANT, CHARCONSTANT, STRINGCONSTANT)) {
    advanceToNextToken();
    parseExpressionTail();
  } else {
    error("expressão inválida ou incompleta");
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
    error("operador aritmético esperado ('+', '-', '*', '/')");
  }
}

void Parser::parseRelationalOperator() {
  if (checkTokens(EQUIVALENT, DIFF, LE, GE, LT, GT)) {
    advanceToNextToken();
  } else {
    error("operador relacional esperado ('==', '!=', '<', '>', '<=', '>=')");
  }
}

void Parser::parseLogicalOperator() {
  if (checkTokens(AND, OR)) {
    advanceToNextToken();
  } else {
    error("operador lógico esperado ('&&', '||')");
  }
}