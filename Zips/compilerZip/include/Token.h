#ifndef __Token_h
#define __Token_h

#include <string>

enum TokenType {
  // Especiais
  UNDEF,
  END_OF_FILE,

  // Identificadores e literais
  ID,
  INTEGERCONSTANT,
  CHARCONSTANT,
  STRINGCONSTANT,

  // Atributos de caractere
  CH,
  BN,
  B0,

  // Palavras-chave
  VOID,
  CHAR,
  INT,
  IF,
  ELSE,
  WHILE,
  FOR,
  RETURN,

  // Operadores aritméticos
  PLUS,
  MINUS,
  TIMES,
  DIVIDED,

  // Operadores de atribuição/comparação
  ASSIGN,
  EQUIVALENT,
  DIFF,

  // Operadores relacionais
  LE,
  LT,
  GE,
  GT,

  // Operadores lógicos
  AND,
  OR,
  NOT,

  // Separadores
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LBRACE,
  RBRACE,
  COMMA,
  SEMICOLON
};

inline std::string tokenToString(TokenType token) {
  switch (token) {
  case TokenType::ID:
    return "identificador";
  case TokenType::INTEGERCONSTANT:
    return "número inteiro";
  case TokenType::CHARCONSTANT:
    return "caractere";
  case TokenType::STRINGCONSTANT:
    return "string";
  case TokenType::PLUS:
    return "+";
  case TokenType::MINUS:
    return "-";
  case TokenType::TIMES:
    return "*";
  case TokenType::DIVIDED:
    return "/";
  case TokenType::ASSIGN:
    return "=";
  case TokenType::EQUIVALENT:
    return "==";
  case TokenType::DIFF:
    return "!=";
  case TokenType::LE:
    return "<=";
  case TokenType::LT:
    return "<";
  case TokenType::GE:
    return ">=";
  case TokenType::GT:
    return ">";
  case TokenType::AND:
    return "&&";
  case TokenType::OR:
    return "||";
  case TokenType::NOT:
    return "!";
  case TokenType::LPAREN:
    return "(";
  case TokenType::RPAREN:
    return ")";
  case TokenType::LBRACKET:
    return "[";
  case TokenType::RBRACKET:
    return "]";
  case TokenType::LBRACE:
    return "{";
  case TokenType::RBRACE:
    return "}";
  case TokenType::COMMA:
    return ",";
  case TokenType::SEMICOLON:
    return ";";
  case TokenType::END_OF_FILE:
    return "fim de arquivo";
  default:
    return "token desconhecido";
  }
}

class Token {
public:
  TokenType name;
  TokenType attribute;
  std::string lexeme;

  Token(TokenType n = TokenType::UNDEF, TokenType attr = TokenType::UNDEF,
        std::string l = "")
      : name(n), attribute(attr), lexeme(l) {}

  Token(TokenType n, std::string l)
      : name(n), attribute(TokenType::UNDEF), lexeme(l) {}
};

#endif