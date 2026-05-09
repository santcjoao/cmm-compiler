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
    Token* tok; // ou excluir essa var ou substituir o restante dos returns por ela.
    string lexeme;

    while(true)
    {
        // ignoring spaces -> padrão 10
        while(isspace(input[pos]))
            pos++;
        
        // digit and integer const
        if (isdigit(input[pos]))
        {
            if (!isdigit(input[pos + 1]))
            {
                pos++;
                tok = new Token(DIGIT);
                return tok;
            }
            
            while (isdigit(input[pos]))
                pos++;    
            tok = new Token(INTEGERCONSTANT);
            return tok;
        }

        // Char

        // string
        if (input[pos] == '"')
        {
            pos++;
            while (isprint(input[pos]))
            {
                if (input[pos] == '"')
                {
                    pos++;
                    break; // sai do while
                }
                pos++;
            }
            tok = new Token(STRINGCONSTANT); 
            return tok;
        }

        // separadores
        if(ispunct(input[pos]))
        {
            switch(input[pos])
            {
                case '(':
                    return new Token(SEPARATOR, LPAREN);
                    break;
                case ')':
                    return new Token(SEPARATOR, RPAREN);
                    break;
                case '[':
                    return new Token(SEPARATOR, LBRACKET);
                    break;
                case ']':
                    return new Token(SEPARATOR, RBRACKET);
                    break;
                case '{':
                    return new Token(SEPARATOR, LBRACE);
                    break;
                case '}':
                    return new Token(SEPARATOR, RBRACE);
                    break;
                case ',':
                    return new Token(SEPARATOR, COMMA);
                    break;
                case ';':
                    return new Token(SEPARATOR, SEMICOLON);
                    break;
                default:
                    // não reconhece outros: inserir erro?
                    lexicalError("LexiSeparador não está na lista de separadores aceitáveis");
                    break;
            }
        }
    }

    return tok;
 
}

void 
Scanner::lexicalError(string msg)
{
    cout << "Linha "<< line << ": " << msg << endl;
    
    exit(EXIT_FAILURE);
}
