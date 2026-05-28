#ifndef __Parser_h
#define __Parser_h

#include "Scanner.h"

class Parser
{
	private:
		Scanner* scanner;
		Token* lToken;

		void advance();
		void match(int);
		
	public:
		Parser(string);
		void run();
        void program();
        void varDeclaration();
        void function();
        //Continuar....
        void error(string);
};

#endif