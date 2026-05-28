#include <fstream>
#include "Token.h"

class Scanner 
{
    private: 
        string input;   //  Armazena o texto de entrada
        int pos;        //  Posição atual
        int line;
    
    public:
        //  Construtor
        Scanner(string);

        int getLine();
    
        //  Método que retorna o próximo token da entrada
        Token* nextToken();        
    
        //  Método para manipular erros
        void lexicalError(string);

        /// Helpers de inspeção de estado
        char nextPeek();
        char currentPeek();
        
        bool isAtEnd();
        void advance(int steps = 1);
        
        bool matchCurrent(char expected);
        bool matchNext(char expected);

        /// Helper para pular espaços e comentários
        void skipWhitespaceAndComments();

        /// Helpers de criação de tokens
        string extractLexeme(int startPos);
        Token* makeToken(int name, int attribute = UNDEF, string lexeme = "");

        /// Helpers de leitura de tokens
        Token* readNumber();

        Token* readCharLiteral();
        Token* readStringLiteral();
        Token* readIdentifierOrKeyword();

        Token* readOperator();
        Token* readSeparator();
};
