#include <iostream>
#include <ctype.h>//Funções de caracteres
#include <string>

using namespace std;

enum Names 
{
    //Continuar
    //Nomes e atributos dos tokens da linguagem
    UNDEF,
    DIGIT,
    ID,
    INTEGERCONSTANT,
    CHARCONSTANT,
    CH, // ch
    BN, // \n
    B0, // \0
    STRINGCONSTANT,
    OPERATOR, 
    PLUS, // +
    MINUS, // -
    TIMES, // *
    DIVIDED, // /
    ASSIGN, // =
    EQUIVALENT, // ==
    DIFF, // !=
    LE, // <=
    LT, // <
    GE, // >=
    GT, // >
    AND, // &&
    OR, // ||
    NOT, // !
    SEPARATOR,
    LPAREN, // (
    RPAREN, // )
    LBRACKET, // [
    RBRACKET, // ]
    LBRACE, // {
    RBRACE, // }
    COMMA, // ,
    SEMICOLON, // ;
    END_OF_FILE
};

class Token 
{
    public: 
        int name;
        int attribute;
        string lexeme;
    
        Token(int name)
        {
            this->name = name;
            attribute = UNDEF;
        }

        Token(int name, string l)
        {
            this->name = name;
            attribute = UNDEF;
            lexeme = l;
        }
        
        Token(int name, int attr)
        {
            this->name = name;
            attribute = attr;
        }
};
