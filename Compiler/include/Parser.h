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
		void a();
		void b();
		void c();
		void d();
		void e();
		void f();
		void g();
		void h();
		void i();
		void j();
		void k();
		void l();
		void m();
		void n();
        void function();
        void varDeclaration();
		void type();
		void paramTypes();
		void statement();
		void assign();
		void aL();
		void expression();
		void eL();
		void binOp();
		void relOp();
		void rL();
		void logOp();
        //Continuar....
        void error(string);
};

#endif