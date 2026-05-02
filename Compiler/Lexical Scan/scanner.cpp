#include "scanner.h"    

//Construtor que recebe uma string com o nome do arquivo 
//de entrada e preenche input com seu conteúdo.
Scanner::Scanner(string input)
{
    /*this->input = input;
    cout << "Entrada: " << input << endl << "Tamanho: " 
         << input.length() << endl;*/
    pos = 0;
    line = 1;

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

    //A próxima linha deve ser comentada posteriormente.
    //Ela é utilizada apenas para verificar se o 
    //preenchimento de input foi feito corretamente.
    cout << this->input;

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
    int state = 0;
    //TODO
    while(true)
    {
        // ignoring spaces
        while(isspace(input[pos]))
            pos++;
        
        //CONSIDERAR FAZER SWITCH A PARTIR DAQUI
        
        // digit and integer const
        if (isdigit(input[pos]))
        {
            while (isdigit(input[pos]))
            {
                
            }
        }

        // string
        if (isprint(input[pos]))
        {
            while (isprint(input[pos]))
            {

            }
        }

        // separadores
    }

    return tok;
 
}

void 
Scanner::lexicalError(string msg)
{
    cout << "Linha "<< line << ": " << msg << endl;
    
    exit(EXIT_FAILURE);
}
