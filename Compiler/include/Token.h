#ifndef __Token_h
#define __Token_h

#include <ctype.h>
#include <iostream>
#include <string>

using namespace std;

enum Names {
  UNDEF,
  ID,
  INTEGERCONSTANT,
  CHARCONSTANT,
  CH,
  BN,
  B0,
  STRINGCONSTANT,
  PLUS,
  MINUS,
  TIMES,
  DIVIDED,
  ASSIGN,
  EQUIVALENT,
  DIFF,
  LE,
  LT,
  GE,
  GT,
  AND,
  OR,
  NOT,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LBRACE,
  RBRACE,
  COMMA,
  SEMICOLON,
  END_OF_FILE
};

inline string tokenToString(int token) {
  switch (token) {
  case ID:
    return "identificador";
  case INTEGERCONSTANT:
    return "número inteiro";
  case CHARCONSTANT:
    return "caractere";
  case STRINGCONSTANT:
    return "string";
  case PLUS:
    return "+";
  case MINUS:
    return "-";
  case TIMES:
    return "*";
  case DIVIDED:
    return "/";
  case ASSIGN:
    return "=";
  case EQUIVALENT:
    return "==";
  case DIFF:
    return "!=";
  case LE:
    return "<=";
  case LT:
    return "<";
  case GE:
    return ">=";
  case GT:
    return ">";
  case AND:
    return "&&";
  case OR:
    return "||";
  case NOT:
    return "!";
  case LPAREN:
    return "(";
  case RPAREN:
    return ")";
  case LBRACKET:
    return "[";
  case RBRACKET:
    return "]";
  case LBRACE:
    return "{";
  case RBRACE:
    return "}";
  case COMMA:
    return ",";
  case SEMICOLON:
    return ";";
  case END_OF_FILE:
    return "fim de arquivo";
  default:
    return "token desconhecido";
  }
}

class Token {
public:
  int name;
  int attribute;
  string lexeme;

  Token(int name, int attr = UNDEF, string l = "")
      : name(name), attribute(attr), lexeme(l) {}
};

#endif