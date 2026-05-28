#include "Lexer/Scanner.h"

const string tokenNames[] = {
    "UNDEF", "LETTER", "DIGIT", "ID", "INTEGERCONSTANT", "CHARCONSTANT", "CH", "BN", "B0",
    "STRINGCONSTANT", "OPERATOR", "PLUS", "MINUS", "TIMES", "DIVIDED", "ASSIGN", "EQUIVALENT",
    "DIFF", "LE", "LT", "GE", "GT", "AND", "OR", "NOT", "SEPARATOR", "LPAREN", "RPAREN",
    "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "COMMA", "SEMICOLON", "END_OF_FILE"
};

int main(int argc, char* argv[]) 
{
    //Verifica se foi executado corretamente
    //Essa main espera receber o nome do arquivo a ser
    //executado na linha de comando.
    if (argc != 2)
    {
        cout << "Uso: ./compiler nome_arquivo.cmm\n";
        return 1;
    }

    string input;
    
    //getline(cin, input);

    Scanner* scanner = new Scanner(argv[1]);
    
    Token* t;
    
    do
    {
        t = scanner->nextToken();
        
        cout << "<" << tokenNames[t->name];
        
        // Exibir o lexema se houver
        if (!t->lexeme.empty()) {
            cout << ", \"" << t->lexeme << "\"";
        }
        
        cout << "> ";
        
    }while (t->name != END_OF_FILE);

    cout << endl; // Pra quebrar a linha no fim

    delete scanner;
    
    return 0;
}