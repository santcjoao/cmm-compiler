#include "../include/Parser.h"

Parser::Parser(string input) { scanner = new Scanner(input); }

void Parser::run() {
  advanceToNextToken();
  program();
  cout << "Compilação encerrada.\n";
}

void Parser::program() {
  parseFunctionList();
  consumeExpectedToken(END_OF_FILE);
}

void Parser::advanceToNextToken() { lToken = scanner->nextToken(); }

void Parser::consumeExpectedToken(int Token) {
  if (checkSingleToken(Token)) {
    advanceToNextToken();
  } else {
    string expected = tokenToString(Token);
    string found =
        lToken->lexeme.empty() ? tokenToString(lToken->name) : lToken->lexeme;
    error("esperado '" + expected + "', mas recebemos '" + found + "'");
  }
}

bool Parser::isStartOfExpression() {
  return (checkTokens(COMMA, NOT, ID, LPAREN, INTEGERCONSTANT, CHARCONSTANT,
                      STRINGCONSTANT));
}

bool Parser::isStartOfStatement() {
  return (checkKeywords(IF, WHILE, FOR, RETURN) ||
          checkTokens(ID, LBRACE, SEMICOLON));
}

void Parser::error(string str) {
  int line = scanner->getTokenLine();
  int col = scanner->getTokenColumn();

  string lexeme =
      lToken->lexeme.empty() ? tokenToString(lToken->name) : lToken->lexeme;
  cout << "arquivo.cmm:" << line << ":" << col << ": erro: " << str
       << " (no token: '" << lexeme << "')\n";

  string sourceLine = scanner->getLineContent(line);
  if (!sourceLine.empty()) {
    cout << "   " << line << " | " << sourceLine << "\n";
    int offset = 3 + to_string(line).length() + 3 + (col > 0 ? col - 1 : 0);
    cout << string(offset, ' ') << "^\n";
  }

  exit(EXIT_FAILURE);
}

void Parser::parseFunctionList() {
  if (checkKeywords(VOID, CHAR, INT)) {
    parseFunction();
    parseFunctionList();
  }
}

void Parser::parseFunction() {
  if (checkKeywords(VOID))
    advanceToNextToken();
  else
    parseType();
  consumeExpectedToken(ID);
  consumeExpectedToken(LPAREN);
  parseParamTypes();
  consumeExpectedToken(RPAREN);
  consumeExpectedToken(LBRACE);
  parseLocalDeclarations();
  parseStatementList();
  consumeExpectedToken(RBRACE);
}

void Parser::parseParamTypes() {
  if (checkKeywords(VOID))
    advanceToNextToken();
  else if (checkKeywords(CHAR, INT)) {
    advanceToNextToken();
    consumeExpectedToken(ID);
    parseArrayBrackets();
    parseMoreParameters();
  } else
    error("Tipo ou 'void' esperado");
}

void Parser::parseMoreParameters() {
  if (checkTokens(COMMA)) {
    advanceToNextToken();
    parseType();
    consumeExpectedToken(ID);
    parseArrayBrackets();
    parseMoreParameters();
  }
}

void Parser::parseType() {
  if (checkKeywords(CHAR, INT))
    advanceToNextToken();
  else
    error("Tipo (char, int) esperado");
}

void Parser::parseLocalDeclarations() {
  if (checkKeywords(CHAR, INT)) {
    parseType();
    parseVariableDeclaration();
    parseMoreVariables();
    consumeExpectedToken(SEMICOLON);
    parseLocalDeclarations();
  }
}

void Parser::parseVariableDeclaration() {
  consumeExpectedToken(ID);
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
  if (checkKeywords(IF)) {
    advanceToNextToken();
    consumeExpectedToken(LPAREN);
    parseExpression();
    consumeExpectedToken(RPAREN);
    parseStatement();
    parseOptionalElse();
  } else if (checkKeywords(WHILE)) {
    advanceToNextToken();
    consumeExpectedToken(LPAREN);
    parseExpression();
    consumeExpectedToken(RPAREN);
    parseStatement();
  } else if (checkKeywords(FOR)) {
    advanceToNextToken();
    consumeExpectedToken(LPAREN);
    parseOptionalAssignment();
    consumeExpectedToken(SEMICOLON);
    parseOptionalExpression();
    consumeExpectedToken(SEMICOLON);
    parseOptionalAssignment();
    consumeExpectedToken(RPAREN);
    parseStatement();
  } else if (checkKeywords(RETURN)) {
    advanceToNextToken();
    parseOptionalExpression();
    consumeExpectedToken(SEMICOLON);
  } else if (checkTokens(ID)) {
    if (checkKeywords(CHAR, INT, VOID)) {
      error("Declarações de variáveis devem vir antes das instruções/comandos "
            "do bloco");
    }
    advanceToNextToken();
    parseAssignmentOrCall();
  } else if (checkTokens(LBRACE)) {
    advanceToNextToken();
    parseStatementList();
    consumeExpectedToken(RBRACE);
  } else if (checkTokens(SEMICOLON))
    advanceToNextToken();
  else
    error("Statement (comando) inválido");
}

void Parser::parseStatementList() {
  if (isStartOfStatement()) {
    parseStatement();
    parseStatementList();
  }
}

void Parser::parseOptionalElse() {
  if (checkKeywords(ELSE)) {
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
  consumeExpectedToken(ID);
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
    error("Atribuição ou chamada de função esperada");
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
    advanceToNextToken();
    parseIdentifierSuffix();
    parseExpressionTail();
  } else if (checkTokens(INTEGERCONSTANT, CHARCONSTANT, STRINGCONSTANT)) {
    advanceToNextToken();
    parseExpressionTail();
  } else {
    string found =
        lToken->lexeme.empty() ? tokenToString(lToken->name) : lToken->lexeme;
    error("Expressão mal formulada (token inesperado: '" + found + "')");
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
    error("Operador aritmético esperado");
  }
}

void Parser::parseRelationalOperator() {
  if (checkTokens(EQUIVALENT, DIFF, LE, GE, LT, GT)) {
    advanceToNextToken();
  } else {
    error("Operador de comparação esperado");
  }
}

void Parser::parseLogicalOperator() {
  if (checkTokens(AND, OR)) {
    advanceToNextToken();
  } else {
    error("Operador AND ou OR esperado");
  }
}
