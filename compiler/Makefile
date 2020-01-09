all: compiler

bison: bison.y
	bison -t -d bison.y

lexer: lexer.l
	flex lexer.l

compiler: lexer bison
	g++ -std=c++11 -o kompiler lex.yy.c bison.tab.c compiler.cpp conditions.cpp expressions.cpp loops.cpp

clean:
	rm -f compiler lex.yy.c bison.tab.c bison.tab.h 
