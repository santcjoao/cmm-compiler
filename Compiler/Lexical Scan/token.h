#include <iostream>
#include <ctype.h>//Funções de caracteres
#include <string>

using namespace std;

enum Names 
{
    //Continuar
    //Nomes e atributos dos tokens da linguagem
    UNDEF,  // - 0
    LETTER, // a-z, A-Z - 1
    DIGIT, // - 2
    ID, // 3
    INTEGERCONSTANT, // 4
    CHARCONSTANT, // 5
    CH, // ch - 6
    BN, // \n - 7
    B0, // \0 - 8
    STRINGCONSTANT, // 9
    OPERATOR, // 10
    PLUS, // + - 11
    MINUS, // - 12
    TIMES, // * - 13
    DIVIDED, // - 14
    ASSIGN, // = - 15
    EQUIVALENT, // == - 16
    DIFF, // != - 17
    LE, // <= - 18
    LT, // < - 19
    GE, // >= - 20
    GT, // > - 21
    AND, // && - 22
    OR, // || - 23
    NOT, // ! -24
    SEPARATOR, // - 25
    LPAREN, // ( - 26
    RPAREN, // ) - 27
    LBRACKET, // [ - 28
    RBRACKET, // ] - 29
    LBRACE, // { - 30
    RBRACE, // } - 31
    COMMA, // , - 32
    SEMICOLON, // ; - 33
    END_OF_FILE // - 34
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
            this->lexeme = l;
        }
        
        Token(int name, int attr)
        {
            this->name = name;
            attribute = attr;
        }
};
