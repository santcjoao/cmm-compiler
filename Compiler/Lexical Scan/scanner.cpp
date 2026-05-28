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

        //end of file
        if (input[pos] == '\0')
        {
            tok = new Token(END_OF_FILE);
            return tok;
        }

        //ignore coments
        if(input[pos] == '/' && input[pos + 1] == '/')
        {
            pos += 2;
            while (input[pos] != '\n' && input[pos] != '\0')
                pos++;
            line++; //vai pra proxima linha 
            continue;
        }
        else if (input[pos] == '/' && input[pos + 1] == '*')
        {
            pos += 2;
            while (!(input[pos] == '*' && input[pos + 1] == '/') && input[pos] != '\0')
            {
                if (input[pos] == '\n')
                    line++;
                pos++;
            }
            if (input[pos] != '\0')
                pos += 2;
            else
                lexicalError("Comentário não fechado.");
            continue;
        }

        //letter
        if(isalpha(input[pos]))
        {
            if (!isalpha(input[pos + 1]))
            {
                pos++;
                tok = new Token(LETTER);
                return tok;
            }

            //ID
            while (isalpha(input[pos]) || isdigit(input[pos]) || input[pos] == '_')
                pos++;
            tok = new Token(ID);
            return tok;


        }
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
        if(input[pos] == '\'') // 'a' ou '\n' ou '\0'
        {
            pos++;
            if (isprint(input[pos]) && input[pos] != '\'' && input[pos] != '\\')
            {
                pos++;
                if (input[pos] == '\'')
                {
                    pos++;
                    tok = new Token(CH);
                    return tok;
                }
            }
            else if (input[pos] == '\\') //'\n'
            {
                pos++;
                if (input[pos] == 'n')
                {
                    pos++;
                    if (input[pos] == '\'')
                    {
                        pos++;
                        tok = new Token(BN);
                        return tok;
                    }
                }
                else if (input[pos] == '0') //'\0'
                {
                    pos++;
                    if (input[pos] == '\'')
                    {
                        pos++;
                        tok = new Token(B0);
                        return tok;
                    }
                }
            }

            //tem que ver caso não siga o formato correto, precisa dar erro D:
        }


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

        //operadores
        if(ispunct(input[pos]))
        {
            switch(input[pos])
            {
                case '+':
                    pos++;
                    return new Token(OPERATOR, PLUS);
                case '-':
                    pos++;
                    return new Token(OPERATOR, MINUS);
                case '*':
                    pos++;
                    return new Token(OPERATOR, TIMES);
                case '/':
                    pos++;
                    return new Token(OPERATOR, DIVIDED);
                case '=':
                    pos++;
                    if (input[pos] == '=')
                    {
                        pos++;
                        return new Token(OPERATOR, EQUIVALENT);
                    }
                    return new Token(OPERATOR, ASSIGN);
                case '!':
                    pos++;
                    if (input[pos] == '=')
                    {
                        pos++;
                        return new Token(OPERATOR, DIFF);
                    }
                    return new Token(OPERATOR, NOT);
                case '<':
                    pos++;
                    if (input[pos] == '=')                    
                    {
                        pos++;
                        return new Token(OPERATOR, LE);
                    }
                    return new Token(OPERATOR, LT);
                case '>':
                    pos++;
                    if (input[pos] == '=')
                    {
                        pos++;
                        return new Token(OPERATOR, GE);
                    }
                    return new Token(OPERATOR, GT);
                case '&':
                    pos++;
                    if (input[pos] == '&')
                    {
                        pos++;
                        return new Token(OPERATOR, AND);
                    }
                    
                    lexicalError("LexiOperador '&' não é um operador válido.");
                    break;

                case '|':
                    pos++;
                    if (input[pos] == '|')
                    {
                        pos++;
                        return new Token(OPERATOR, OR);
                    }
                    lexicalError("LexiOperador '|' não é um operador válido."); 
                    break;
                default:
                    lexicalError("LexiOperador não está na lista de operadores aceitáveis");
                    break;
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
}

void 
Scanner::lexicalError(string msg)
{
    cout << "Linha "<< line << ": " << msg << endl;
    
    exit(EXIT_FAILURE);
}
