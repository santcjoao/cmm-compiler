#include "../include/Scanner.h"

//  Construtor: Recebe o nome do arquivo de entrada, lê seu conteúdo e o
//  armazena na variável input.
Scanner::Scanner(string input) {
  /*this->input = input;
  cout << "Entrada: " << input << endl << "Tamanho: "
       << input.length() << endl;*/
  pos = 0;
  line = 1;
  column = 1;
  tokenLine = 1;
  tokenColumn = 1;

  ifstream inputFile(input, ios::in);
  string line;

  if (inputFile.is_open()) {
    while (getline(inputFile, line)) {
      this->input.append(line + '\n');
    }
    inputFile.close();
  } else
    cout << "Unable to open file\n";

  // A próxima linha deve ser comentada posteriormente.
  // Ela é utilizada apenas para verificar se o
  // preenchimento de input foi feito corretamente.
  cout << this->input;
}

int Scanner::getLine() { return line; }
int Scanner::getColumn() { return column; }
int Scanner::getTokenLine() { return tokenLine; }
int Scanner::getTokenColumn() { return tokenColumn; }

string Scanner::getLineContent(int lineNumber) {
  int current = 1;
  int start = 0;
  for (int i = 0; i < input.length(); i++) {
    if (current == lineNumber) {
      int end = i;
      while (end < input.length() && input[end] != '\n') {
        end++;
      }
      return input.substr(start, end - start);
    }
    if (input[i] == '\n') {
      current++;
      start = i + 1;
    }
  }
  return "";
}
void Scanner::lexicalError(string msg) {
  cout << "Linha " << line << ": " << msg << endl;

  exit(EXIT_FAILURE);
}

Token *Scanner::nextToken() {
  skipWhitespaceAndComments();
  if (isAtEnd()) {
    return makeToken(END_OF_FILE);
  }
  tokenLine = line;
  tokenColumn = column;

  // 3. Inspeciona o caractere atual para decidir que token ler
  char c = currentPeek();

  // Identificadores ou palavras-chave começam com letra
  if (isalpha(c)) {
    return readIdentifierOrKeyword();
  }

  // Números começam com dígito
  if (isdigit(c)) {
    return readNumber();
  }

  // Char literal começa com aspa simples
  if (c == '\'') {
    return readCharLiteral();
  }

  // String literal começa com aspa dupla
  if (c == '"') {
    return readStringLiteral();
  }

  // Operadores
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '!' ||
      c == '<' || c == '>' || c == '&' || c == '|') {
    return readOperator();
  }

  // Separadores
  if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
      c == ',' || c == ';') {
    return readSeparator();
  }

  // Se não entrou em nenhum fallback conhecido, lança erro léxico
  string msg = "Caractere não reconhecido: '";
  msg += c;
  msg += "'";
  lexicalError(msg);

  return nullptr;
}

// Boolean helpers para inspeção e correspondência de caracteres
bool Scanner::isAtEnd() { return pos >= input.length(); }
bool Scanner::matchCurrent(char expected) {
  if (isAtEnd())
    return false;
  if (currentPeek() != expected)
    return false;

  return true;
}
bool Scanner::matchNext(char expected) {
  if (isAtEnd())
    return false;
  if (nextPeek() != expected)
    return false;

  return true;
}

// Helpers para inspeção de caracteres
char Scanner::currentPeek() {
  if (isAtEnd())
    return '\0';
  return input[pos];
}
char Scanner::nextPeek() {
  if (pos + 1 >= input.length())
    return '\0';
  return input[pos + 1];
}
void Scanner::advance(int steps) {
  for (int i = 0; i < steps; i++) {
    if (pos < input.length()) {
      if (input[pos] == '\n') {
        line++;
        column = 1;
      } else {
        column++;
      }
      pos++;
    }
  }
}

// Helpers para criar tokens
Token *Scanner::makeToken(int name, int attribute, string lexeme) {
  return new Token(name, attribute, lexeme);
}
string Scanner::extractLexeme(int startPos, int endPos) {
  return input.substr(startPos, endPos - startPos);
}

// Helper para pular espaços e comentários
void Scanner::skipWhitespaceAndComments() {
  while (true) {
    char c = currentPeek();

    if (isspace(c)) {
      advance();
    } else if (c == '/' && nextPeek() == '/') {
      // Single-line comment
      advance(2); // Consume '//'

      // Avança enquanto nao for quebra de linha e nao acabar
      while (!matchCurrent('\n') && !isAtEnd()) {
        advance();
      }

      // Se fechou por quebra, consome o \n
      if (!isAtEnd()) {
        advance();
      }
    } else if (c == '/' && nextPeek() == '*') {
      // Multi-line comment
      advance(2); // Consume '/*'

      // Continua pulando até achar '*/'
      while (!(matchCurrent('*') && matchNext('/')) && !isAtEnd()) {
        advance();
      }

      if (!isAtEnd()) {
        advance(2); // Consume '*/'
      } else {
        lexicalError("Comentário de bloco não fechado.");
      }
    } else {
      break;
    }
  }
}

// Helpers para leitura de tokens específicos
Token *Scanner::readNumber() {
  int startPos = pos;

  while (isdigit(currentPeek())) {
    advance();
  }

  string lexeme = extractLexeme(startPos, pos);
  return makeToken(INTEGERCONSTANT, UNDEF, lexeme);
}
Token *Scanner::readCharLiteral() {
  int startPos = pos;
  advance(); // Consume a aspa simples de abertura '\''

  if (matchCurrent('\\')) // Escape character
  {
    advance();
    if (matchCurrent('n')) {
      advance();
      if (matchCurrent('\'')) {
        advance();
        return makeToken(CHARCONSTANT, BN, "\\n");
      }
    } else if (matchCurrent('0')) {
      advance();
      if (matchCurrent('\'')) {
        advance();
        return makeToken(CHARCONSTANT, B0, "\\0");
      }
    } else {
      lexicalError("Tratamento de escape de caractere inválido.");
    }
  } else if (isprint(currentPeek()) && currentPeek() != '\'' &&
             currentPeek() != '\\') {
    advance(); // Consome o caractere
    if (matchCurrent('\'')) {
      advance(); // Consome a aspa final
      string lexeme = extractLexeme(startPos + 1, pos - 1);
      return makeToken(CHARCONSTANT, CH, lexeme);
    }
  }

  lexicalError("Literal de caractere mal formado.");
  return nullptr;
}
Token *Scanner::readStringLiteral() {
  int startPos = pos;
  advance(); // Consume as aspas duplas de abertura '"'

  while (!isAtEnd() && currentPeek() != '"') {
    advance();
  }

  if (isAtEnd()) {
    lexicalError("String não fechada antes do fim do arquivo.");
  }

  advance(); // Consume as aspas finais
  // Extrai a string ignorando as aspas (inicial e final)
  string lexeme = extractLexeme(startPos + 1, pos - 1);
  return makeToken(STRINGCONSTANT, UNDEF, lexeme);
}
Token *Scanner::readIdentifierOrKeyword() {
  int startPos = pos;

  while (isalpha(currentPeek()) || isdigit(currentPeek()) ||
         currentPeek() == '_') {
    advance();
  }

  string lexeme = extractLexeme(startPos, pos);

  if (lexeme == "void")   return makeToken(ID, VOID, lexeme);
  if (lexeme == "int")    return makeToken(ID, INT, lexeme);
  if (lexeme == "char")   return makeToken(ID, CHAR, lexeme);
  if (lexeme == "if")     return makeToken(ID, IF, lexeme);
  if (lexeme == "else")   return makeToken(ID, ELSE, lexeme);
  if (lexeme == "while")  return makeToken(ID, WHILE, lexeme);
  if (lexeme == "for")    return makeToken(ID, FOR, lexeme);
  if (lexeme == "return") return makeToken(ID, RETURN, lexeme);

  return makeToken(ID, UNDEF, lexeme);
}
Token *Scanner::readOperator() {
  int startPos = pos;
  char c = currentPeek();
  advance(); // consome o primeiro char do operador

  switch (c) {
  case '+':
    return makeToken(OPERATOR, PLUS, "+");
  case '-':
    return makeToken(OPERATOR, MINUS, "-");
  case '*':
    return makeToken(OPERATOR, TIMES, "*");
  case '/':
    return makeToken(OPERATOR, DIVIDED, "/");

  case '=':
    if (matchCurrent('=')) {
      advance();
      return makeToken(OPERATOR, EQUIVALENT, "==");
    }
    return makeToken(OPERATOR, ASSIGN, "=");

  case '!':
    if (matchCurrent('=')) {
      advance();
      return makeToken(OPERATOR, DIFF, "!=");
    }
    return makeToken(OPERATOR, NOT, "!");

  case '<':
    if (matchCurrent('=')) {
      advance();
      return makeToken(OPERATOR, LE, "<=");
    }
    return makeToken(OPERATOR, LT, "<");

  case '>':
    if (matchCurrent('=')) {
      advance();
      return makeToken(OPERATOR, GE, ">=");
    }
    return makeToken(OPERATOR, GT, ">");

  case '&':
    if (matchCurrent('&')) {
      advance();
      return makeToken(OPERATOR, AND, "&&");
    }
    lexicalError("Operador '&' incompleto.");
    break;

  case '|':
    if (matchCurrent('|')) {
      advance();
      return makeToken(OPERATOR, OR, "||");
    }
    lexicalError("Operador '|' incompleto.");
    break;

  default:
    lexicalError("Operador não reconhecido.");
    break;
  }

  return nullptr;
}
Token *Scanner::readSeparator() {
  int startPos = pos;
  char c = currentPeek();
  advance(); // Consome o separador

  switch (c) {
  case '(':
    return makeToken(SEPARATOR, LPAREN, "(");
  case ')':
    return makeToken(SEPARATOR, RPAREN, ")");
  case '[':
    return makeToken(SEPARATOR, LBRACKET, "[");
  case ']':
    return makeToken(SEPARATOR, RBRACKET, "]");
  case '{':
    return makeToken(SEPARATOR, LBRACE, "{");
  case '}':
    return makeToken(SEPARATOR, RBRACE, "}");
  case ',':
    return makeToken(SEPARATOR, COMMA, ",");
  case ';':
    return makeToken(SEPARATOR, SEMICOLON, ";");
  default:
    lexicalError("Separador não reconhecido.");
    break;
  }

  return nullptr;
}