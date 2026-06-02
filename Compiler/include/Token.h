#ifndef __Token_h
#define __Token_h

#include <ctype.h>
#include <iostream>
#include <string>

using namespace std;

enum Keywords { VOID = 100, INT, CHAR, IF, ELSE, WHILE, FOR, RETURN };

inline string keywordToString(int kw) {
  switch (kw) {
  case VOID:
    return "void";
  case INT:
    return "int";
  case CHAR:
    return "char";
  case IF:
    return "if";
  case ELSE:
    return "else";
  case WHILE:
    return "while";
  case FOR:
    return "for";
  case RETURN:
    return "return";
  default:
    return "palavra-chave desconhecida";
  }
}

enum Names {
  // Nomes e atributos dos tokens da linguagem
  UNDEF,           // - 0
  LETTER,          // a-z, A-Z - 1
  DIGIT,           // - 2
  ID,              // 3
  INTEGERCONSTANT, // 4
  CHARCONSTANT,    // 5
  CH,              // ch - 6
  BN,              // \n - 7
  B0,              // \0 - 8
  STRINGCONSTANT,  // 9
  OPERATOR,        // 10
  PLUS,            // + - 11
  MINUS,           // - 12
  TIMES,           // * - 13
  DIVIDED,         // - 14
  ASSIGN,          // = - 15
  EQUIVALENT,      // == - 16
  DIFF,            // != - 17
  LE,              // <= - 18
  LT,              // < - 19
  GE,              // >= - 20
  GT,              // > - 21
  AND,             // && - 22
  OR,              // || - 23
  NOT,             // ! -24
  SEPARATOR,       // - 25
  LPAREN,          // ( - 26
  RPAREN,          // ) - 27
  LBRACKET,        // [ - 28
  RBRACKET,        // ] - 29
  LBRACE,          // { - 30
  RBRACE,          // } - 31
  COMMA,           // , - 32
  SEMICOLON,       // ; - 33
  END_OF_FILE      // - 34
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

  Token(int name) {
    this->name = name;
    attribute = UNDEF;
  }

  Token(int name, string l) {
    this->name = name;
    attribute = UNDEF;
    this->lexeme = l;
  }

  Token(int name, int attr) {
    this->name = name;
    attribute = attr;
  }

  Token(int name, int attr, string l) {
    this->name = name;
    attribute = attr;
    this->lexeme = l;
  }
};

#endif