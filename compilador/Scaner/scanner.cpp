#include "scanner.h"    

//Construtor
Scanner::Scanner(string input)
{
    this->buffer = input;
    cout << "Entrada: " << buffer << endl << "Tamanho: " 
         << buffer.length() << endl;
    pos = 0;
}

//Método que retorna o próximo token da entrada
Token* 
Scanner::nextToken()
{
    Token* tok;
    int state = 0;//estado inicial
    string lexeme;

    while (true)
    {
        switch (state)
        {
            case 0://juntando o estado 9, o 12
                if (buffer[pos] == '\0')
                {
                    tok = new Token(END_OF_FILE);
                    return tok;
                }
                else if (buffer[pos] == '<')
                {
                    state = 1;
                    pos++;
                }
                else if (buffer[pos] == '=')
                {
                    state = 5;
                    pos++;
                }
                else if (buffer[pos] == '>')
                {
                    state = 6;
                    pos++;
                }
                else if (isalpha(buffer[pos]))
                {
                    lexeme.push_back(buffer[pos]);
                    state = 10;
                    pos++;
                }
                else if (isdigit(buffer[pos]))
                {
                    lexeme.push_back(buffer[pos]);
                    state = 13;
                    pos++;
                }
                else if (isspace(buffer[pos]))
                {
                    state = 23;
                    pos++;
                }
                else
                    lexicalError();

                break;

            case 1:
                if (buffer[pos] == '=')
                    state = 2;
                else if (buffer[pos] == '>')
                    state = 3;
                else
                    state = 4;
                
                pos++;

                break;
            case 2:
                tok = new Token(RELOP, LE);
                return tok;
            case 3:
                tok = new Token(RELOP, NE);
                return tok;
            case 4:
                pos--;//"Desconsumir" o símbolo
                tok = new Token(RELOP, LT);
                return tok;
            case 5:
                tok = new Token(RELOP, EQ);
                return tok;
            case 6:
                if (buffer[pos] == '=')
                    state = 7;               
                else
                    state = 8;
                
                pos++;

                break;
            case 7:
                tok = new Token(RELOP, GE);
                return tok;
            case 8:
                pos--;
                tok = new Token(RELOP, GT);
                return tok;
            //O CASE 9 NÃO ESTÁ AQUI PORQUE FOI JUNTADO COM O CASE 0
            case 10:
                if (!isalnum(buffer[pos]))
                    state = 11;
                
                lexeme.push_back(buffer[pos]);
                pos++;

                break;
            case 11:
                lexeme.pop_back();//"Desarmazena" o caractere que não pertence ao ID
                pos--;//"Desconsome"

                //Ponto onde verificamos se é palavra reservada

                tok = new Token(ID);
                tok->lexeme = lexeme;

                return tok; 
            //O CASE 12 NÃO ESTÁ AQUI PORQUE FOI JUNTADO COM O CASE 0
            case 13:
                if (buffer[pos] == '.')
                    state = 14;
                else if (buffer[pos] == 'E')
                    state = 16;
                else if (!isdigit(buffer[pos]))
                    state = 20;

                lexeme.push_back(buffer[pos]);
                pos++;

                break;
            case 14:
                if (!isdigit(buffer[pos]))
                    lexicalError();

                lexeme.push_back(buffer[pos]);
                state = 15;
                pos++;

                break;
            case 15:
                if (buffer[pos] == 'E')
                    state = 16;
                else if (!isdigit(buffer[pos]))
                    state = 21;

                lexeme.push_back(buffer[pos]);
                pos++;

                break;
            case 16:
                if (buffer[pos] == '+' || buffer[pos] == '-')
                    state = 17;
                else if (isdigit(buffer[pos]))
                    state = 18;
                else
                    lexicalError();

                lexeme.push_back(buffer[pos]);
                pos++;

                break;
            case 17:
                if (!isdigit(buffer[pos]))
                    lexicalError();

                state = 18;
                lexeme.push_back(buffer[pos]);
                pos++;

                break;
            case 18:
                if (!isdigit(buffer[pos]))
                    state = 19;

                lexeme.push_back(buffer[pos]);
                pos++;

                break;
            case 19:
                lexeme.pop_back();
                pos--;//"Desconsumir"
                tok = new Token(NUMBER, DOUBLE_LITERAL);
                tok->lexeme = lexeme;
                return tok;
            case 20:
                lexeme.pop_back();
                pos--;//"Desconsumir"
                tok = new Token(NUMBER, INTEGER_LITERAL);
                tok->lexeme = lexeme;
                return tok;
            case 21:
                lexeme.pop_back();
                pos--;//"Desconsumir"
                tok = new Token(NUMBER, FLOAT_LITERAL);
                tok->lexeme = lexeme;
                return tok;
            //O CASE 22 NÃO ESTÁ AQUI PORQUE FOI JUNTADO COM O CASE 0
            case 23:
                if (!isspace(buffer[pos]))
                    state = 24;

                pos++;

                break;
            case 24:
                pos--;//"Desconsumir" espaço não em branco

                //Não há o que retornar, só ignoramos espaços em branco
                //Vamos para o estado que começa o processo de
                //reconhecimento de tokens, o zero
                state = 0;

                break;
            default:
                lexicalError();
        }
    }
}//Fim nextToken

/*Scan  ner::nextToken()
{
    Token* tok;
    int state = 0;//estado inicial
    string lexeme;

    while (true)
    {
        switch (state)
        {
            case 0://case 9: case 12: case 22:
                if (input[pos] == '\0')
                {
                    tok = new Token(END_OF_FILE);
                    return tok;
                }
                else if (input[pos] == '<')
                    state = 1;
                else if (input[pos] == '=')
                    state = 5;
                else if (input[pos] == '>')
                    state = 6;
                else if (isalpha(input[pos]))
                {
                    state = 10;
                    lexeme.push_back(input[pos]);
                }
                else if (isdigit(input[pos]))
                {
                    state = 13;
                    lexeme.push_back(input[pos]);
                }
                else if (isspace(input[pos]))
                    state = 23;
                else
                    lexicalError();

                pos++;

                break;

            case 1:
                if (input[pos] == '=')
                    state = 2;
                else if (input[pos] == '>')
                    state = 3;
                else
                    state = 4;

                pos++;

                break;

            case 2:
                tok = new Token(RELOP, LE);
                return tok;

            case 3:
                tok = new Token(RELOP, NE);
                return tok;

            case 4:
                pos--;
                tok = new Token(RELOP, LT);
                return tok;

            case 5:
                tok = new Token(RELOP, EQ);
                return tok;

            case 6:
                if (input[pos] == '=')
                    state = 7;
                else
                    state = 8;

                pos++;

                break;

            case 7:
                tok = new Token(RELOP, GE);
                return tok;

            case 8:
                pos--;
                tok = new Token(RELOP, GT);
                return tok;

            case 10:
                if (!isalnum(input[pos]))
                    state = 11;

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 11:
                pos--;
                lexeme.pop_back();
                tok = new Token(ID);//Não estamos diferenciando IDs de palavras reservadas ainda
                tok->lexeme = lexeme;
                return tok;

            case 13:
                if (input[pos] == '.')
                    state = 14;
                else if (input[pos] == 'E')
                    state = 16;
                else if (!isdigit(input[pos]))
                    state = 20;

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 14:
                if (isdigit(input[pos]))
                    state = 15;
                else
                    lexicalError();

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 15:
                if (input[pos] == 'E')
                    state = 16;
                else if (!isdigit(input[pos]))
                    state = 21;

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 16:
                if (input[pos] == '+' || input[pos] == '-')
                    state = 17;
                else if (isdigit(input[pos]))
                    state = 18;
                else
                    lexicalError();

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 17:
                if (isdigit(input[pos]))
                    state = 18;
                else
                    lexicalError();

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 18:
                if (!isdigit(input[pos]))
                    state = 19;

                lexeme.push_back(input[pos]);
                pos++;

                break;

            case 19:
                pos--;
                lexeme.pop_back();
                tok = new Token(NUMBER, DOUBLE_LITERAL);
                tok->lexeme = lexeme;
                return tok;

            case 20:
                pos--;
                lexeme.pop_back();
                tok = new Token(NUMBER, INTEGER_LITERAL);
                tok->lexeme = lexeme;
                return tok;

            case 21:
                pos--;
                lexeme.pop_back();
                tok = new Token(NUMBER, FLOAT_LITERAL);
                tok->lexeme = lexeme;
                return tok;

            case 23:
                if (!isspace(input[pos]))
                    state = 24;

                pos++;

                break;

            case 24:
                pos--;
                state = 0;

                break;

        }
    }
}//Fim nextToken
*/
void 
Scanner::lexicalError()
{
    cout << "Token mal formado\n";
    
    exit(EXIT_FAILURE);
}
