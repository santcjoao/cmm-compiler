#include "scanner.h"    

//Construtor
Scanner::Scanner(string input, SymbolTable* table)
{
    pos = 0;
    line = 1;

    st = table;

    ifstream inputFile(input, ios::in);
    string line;

    if (inputFile.is_open())
    {
        while (getline(inputFile,line) )
        {
            this->input.append(line + '\n');
        }
        inputFile.close();
    }
    else 
        cout << "Unable to open file\n"; 

    //cout << this->input;

}

int
Scanner::getLine()
{
    return line;
}

//Método que retorna o próximo token da entrada
Token* 
Scanner::nextToken()
{
    Token* tok;
    string lexeme;
    
    //TODO
    //....
    //Trecho que reconhece Identificadores
    if (isalpha(input[pos]))
    {
        lexeme.push_back(input[pos]);
        pos++;
        while (isalnum(input[pos]) || input[pos] == '_')
        {
            lexeme.push_back(input[pos]);
            pos++;
        }

        //Pesquisar na tabela de palavras reservadas
        //caso encontrem, retornem o token adequado,
        //caso contrário, é ID
        STEntry* obj = st->get(lexeme);
        if (!obj)
            tok = new Token(ID, lexeme);
        else 
            tok = new Token(obj->token->name);
    }
    
    //TODO
    //....

    return tok;
 
}

void 
Scanner::lexicalError(string msg)
{
    cout << "Linha "<< line << ": " << msg << endl;
    
    exit(EXIT_FAILURE);
}
