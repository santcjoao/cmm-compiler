#include "../include/Parser.h"

Parser::Parser(string input)
{
	scanner = new Scanner(input);
}

void
Parser::advance()
{
	lToken = scanner->nextToken();
}

void
Parser::match(int t)
{
	if (lToken->name == t || lToken->attribute == t)
		advance();
	else
		error("Erro inesperado");
}

void
Parser::run()
{
	advance();	

	program();
	
	cout << "Compilação encerrada com sucesso!\n";
}

void
Parser::program()
{
	/* assim tbm funcionaria: vamos ponderar pra ver qual queremos.
    while (lToken->name != END_OF_FILE)
        function();
	*/
	a();
    match(END_OF_FILE);
}

void Parser::a()
{
	if (lToken->lexeme == "void" || lToken->lexeme == "char" || lToken->lexeme == "int")
	{
		function();
		a();
	}
	return;
}

void Parser::b()
{
	if (lToken->name == LBRACKET|| lToken->attribute == LBRACKET)
	{
		advance();
		match(INTEGERCONSTANT);
		match(RBRACKET);
	}
	return;
}

void Parser::c()
{
	if (lToken->name == COMMA || lToken->attribute == COMMA)
	{
		advance();
		varDeclaration();
		c();
		match(SEMICOLON);
	}
	return;
}

void Parser::d()
{
	if (lToken->name == LBRACKET || lToken->attribute == LBRACKET)
	{
		advance();
		match(RBRACKET);
	}
	return;
}

void Parser::e()
{
	if (lToken->name == COMMA || lToken->attribute == COMMA)
	{
		advance();
		expression();
		e();
	}
	return;
}

void Parser::f()
{
	// Checking if next token is in the first of Expression
	if (lToken->name == COMMA || lToken->attribute == COMMA
		|| lToken->name == NOT || lToken->attribute == NOT
		|| lToken->name == ID || lToken->attribute == ID
		|| lToken->name == LPAREN || lToken->attribute == LPAREN
		|| lToken->name == INTEGERCONSTANT || lToken->attribute == INTEGERCONSTANT
		|| lToken->name == CHARCONSTANT || lToken->attribute == CHARCONSTANT
		|| lToken->name == STRINGCONSTANT || lToken->attribute == STRINGCONSTANT)
	{
		advance();
		expression();
		e();
	}
	return;
}

void Parser::g()
{
	if (lToken->name == LPAREN || lToken->attribute == LPAREN){
		advance();
		f();
		match(RPAREN);

	}
	else if(lToken->name == LBRACKET || lToken->attribute == LBRACKET){
		advance();
		expression();
		match(RBRACKET);
	}

	return;	
}

void Parser::h()
{
	// Checking if next token is in the first of Statement
	if (lToken->lexeme == "if" || lToken->lexeme == "while" || lToken->lexeme == "for"
		|| lToken->lexeme == "return" || lToken->name == ID || lToken->attribute == ID
		|| lToken->name == LBRACE || lToken->attribute == LBRACE
		|| lToken->name == SEMICOLON || lToken->attribute == SEMICOLON)
	{
		statement();
		h();
	}
	return;
}

void Parser::i()
{
	// TODO
	if (lToken->name == COMMA || lToken->attribute == COMMA)
	{
		advance();
		type();
		match(ID);
		d();
		i();
	}
	return;
}

void Parser::j()
{
	if (lToken->lexeme == "else")
	{
		advance();
		statement();
	}
	return;
}

void Parser::k()
{
	if (lToken->name == LBRACKET || lToken->attribute == LBRACKET)
	{
		advance();
		expression();
		match(RBRACKET);
	}
	return;
}

void Parser::l()
{
	// checking if next token is in the first of Assign
	if (lToken->name == ID || lToken->attribute == ID)
		assign();
	return;
}

void Parser::m()
{
	// Checking if next token is in the first of Expression
	if (lToken->name == COMMA || lToken->attribute == COMMA
		|| lToken->name == NOT || lToken->attribute == NOT
		|| lToken->name == ID || lToken->attribute == ID
		|| lToken->name == LPAREN || lToken->attribute == LPAREN
		|| lToken->name == INTEGERCONSTANT || lToken->attribute == INTEGERCONSTANT
		|| lToken->name == CHARCONSTANT || lToken->attribute == CHARCONSTANT
		|| lToken->name == STRINGCONSTANT || lToken->attribute == STRINGCONSTANT)
	{
		expression();
	}
	return;
}

void Parser::n()
{
	// Cheking if next token is in the first of type
	if (lToken->lexeme == "char" || lToken->lexeme == "int")
	{
		type();
		varDeclaration();
		c();
		n();
	}
	return;
}

void
Parser::function()
{
	// TODO: Check void
	if (lToken->lexeme == "void")
		advance();
	else
    	type();
    match(ID);
    match(LPAREN);
    paramTypes();
    match(RPAREN);
    match(LBRACE);
	n();
	h();
	match(RBRACE);
}

void Parser::varDeclaration()
{
	match(ID);
	b();
}

void Parser::type()
{
	if (lToken->lexeme == "char" || lToken->lexeme == "int")
		advance();
	else
		error("Tipo esperado");
}

void Parser::paramTypes()
{
	if (lToken->lexeme == "void")
		advance();
	else if (lToken->lexeme == "char" || lToken->lexeme == "int")
	{
		advance();
		match(ID);
		d();
		i();
	}
	else
		error("Tipo ou void esperado");
}

void Parser::statement()
{
	if(lToken->lexeme == "if")
	{
		advance();
		match(LPAREN);
		expression();
		match(RPAREN);
		statement();
		j();	
	}
	else if (lToken->lexeme == "while")
	{
		advance();
		match(LPAREN);
		expression();
		match(RPAREN);
		statement();
	}
	else if (lToken->lexeme == "for")
	{
		advance();
		match(LPAREN);
		l();
		match(SEMICOLON);
		m();
		match(SEMICOLON);
		l();
		match(RPAREN);
		statement();
	}
	else if (lToken->lexeme == "return")
	{
		advance();
		m();
		match(SEMICOLON);
	}
	else if (lToken->name == ID || lToken->attribute == ID)
	{
		advance();
		aL();
	}
	else if (lToken->name == LBRACE || lToken->attribute == LBRACE)
	{
		advance();
		h();
		match(RBRACE);
	}
	else if (lToken->name == SEMICOLON || lToken->attribute == SEMICOLON)
		advance();
	else
		error("Statement inválido");
}

void Parser::assign()
{
	match(ID);
	k();
	match(ASSIGN);
	expression();
}

void Parser::aL()
{
	if (lToken->name == LPAREN || lToken->attribute == LPAREN)
	{
		advance();
		f();
		match(RPAREN);
		match(SEMICOLON);
	}
	else if (lToken->name == LBRACKET || lToken->attribute == LBRACKET
			|| lToken->name == ASSIGN || lToken->attribute == ASSIGN)
	{
		k();
		match(ASSIGN);
		expression();
	}
	else
		error("Atribuição ou chamada de função esperada");
}

void Parser::expression()
{
	if (lToken->name == MINUS || lToken->attribute == MINUS
		|| lToken->name == NOT || lToken->attribute == NOT
		|| lToken->name == LPAREN || lToken->attribute == LPAREN)
	{
		advance();
		expression();
		eL();
	}

	else if( lToken->name == ID || lToken->attribute == ID){
		advance();
		g();
		eL();
	}
	else if(lToken->name == INTEGERCONSTANT || lToken->attribute == INTEGERCONSTANT
		|| lToken->name == CHARCONSTANT || lToken->attribute == CHARCONSTANT
		|| lToken->name == STRINGCONSTANT || lToken->attribute == STRINGCONSTANT)
	{
		advance();
		eL();
	}
	else{
		error("Expressão mal formulada");
	}
}

void Parser::eL()
{
	// Checking if next token is in the first of BinOP
	if(lToken->name == PLUS || lToken->attribute == PLUS
	  || lToken->name == MINUS || lToken->attribute == MINUS
	  || lToken->name == TIMES || lToken->attribute == TIMES
	  || lToken->name == DIVIDED || lToken->attribute == DIVIDED	
	)
	{
		advance();
		eL();
	}
	// Checking if next token is in the first of Relop
	else if(lToken->name == EQUIVALENT || lToken->attribute == EQUIVALENT
			|| lToken->name == DIFF || lToken->attribute == DIFF
			|| lToken->name == LE || lToken->attribute == LE
			|| lToken->name == GE || lToken->attribute == GE
			|| lToken->name == LT || lToken->attribute == LT
			|| lToken->name == GT || lToken->attribute == GT
		)
		{
			advance();
			eL();
		}
	// Checking if next token is in the first of LogOp
	else if(lToken->name == AND || lToken->attribute == AND
			|| lToken->name == OR || lToken->attribute == OR
	)
	{
		advance();
		eL();
	}
	return;
}	

void Parser::binOp()
{
	// Checking if next token is in the first of BinOP
	if(lToken->name == PLUS || lToken->attribute == PLUS
	  || lToken->name == MINUS || lToken->attribute == MINUS
	  || lToken->name == TIMES || lToken->attribute == TIMES
	  || lToken->name == DIVIDED || lToken->attribute == DIVIDED	
	)
	{
		advance();
	}
	else
	{
		error("Operador aritmético esperado");
	}
	return;
}

void Parser::relOp()
{
	// Checking if next token is in the first of Relop
	if(lToken->name == EQUIVALENT || lToken->attribute == EQUIVALENT
			|| lToken->name == DIFF || lToken->attribute == DIFF
			|| lToken->name == LE || lToken->attribute == LE
			|| lToken->name == GE || lToken->attribute == GE
		)
		{
			advance();
		}
	else if(lToken->name == LT || lToken->attribute == LT
			|| lToken->name == GT || lToken->attribute == GT)
		{
			advance();
			rL();
		}
	else
	{
		error("Operador de comparação esperado");
	}
	return;
}

void Parser::rL()
{
	// Checking if next token is in the first of Relop'
		if(lToken->name == ASSIGN || lToken->attribute == ASSIGN 
		  || lToken->name == VOID || lToken->attribute == VOID){
			advance();
		}

		return;
}

void Parser::logOp()
{
	// Checking if next token is in the first of LogOp
	if(lToken->name == AND || lToken->attribute == AND
			|| lToken->name == OR || lToken->attribute == OR
	)
	{
		advance();
	}
	else{
		error("Operador AND ou OR esperado");
	}
	return;
}

void
Parser::error(string str)
{
	cout << "Linha " << scanner->getLine() << ": " << str << endl;

	exit(EXIT_FAILURE);
}