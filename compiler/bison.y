%{
#include "compilerLogic.hpp"

int yylex();
extern int yylineno;
extern FILE *yyin;
void yyerror(const string str);

%}

%define parse.error verbose
%define parse.lac full //%expect 2

%union {
    char* str;
    long long int num;
}

%token <str> NUM IDENTIFIER
%token <str> DECLARE BEG END READ WRITE FOR FROM TO DOWNTO DO WHILE IF THEN ELSE ENDFOR ENDWHILE ENDIF ENDDO
%token <str> ASSIGN NEQ EQ LE GE LEQ GEQ SEM COMMA COL ADD SUB MUL DIV MOD LB RB

%type <str> value
%type <str> identifier

%%
program:
    DECLARE declarations BEG commands END { cout << "halt";}//insert("HALT"); }
    | BEG commands END
    ;

declarations:
    declarations COMMA IDENTIFIER { cout << "declaration1" << endl; }
    | declarations COMMA IDENTIFIER LB NUM COL NUM RB { cout << "declaration2" << endl; }
    | IDENTIFIER { cout << "declaration3" << endl; }
    | IDENTIFIER LB NUM COL NUM RB { cout << "declaration4" << endl; }
    ;

commands:
    commands command
    | command
    ;

command:
    IDENTIFIER ASSIGN expression SEM              { cout << "assign" << endl; }
    | IF condition THEN commands ELSE commands ENDIF { cout << "else" << endl; }  
    | IF condition THEN commands ENDIF { cout << "if" << endl; }            
    | WHILE condition DO commands ENDWHILE { cout << "while" << endl; }
    | DO condition WHILE commands ENDDO         { cout << "do" << endl; }
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR { cout << "for" << endl; }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR { cout << "downfor" << endl; }
    | READ identifier SEM                         { cout << "read" << endl; }
    | WRITE value SEM                             { cout << "write" << endl; }
    ;

expression:
    value                                          { cout << "value" << endl; }
    | value ADD value                              { cout << "add" << endl; }
    | value SUB value                              { cout << "sub" << endl; }
    | value MUL value                              { cout << "mul" << endl; }
    | value DIV value                              { cout << "div" << endl; }
    | value MOD value                              { cout << "mod" << endl; }
    ;

condition:
    value EQ value                                 { cout << "=" << endl; }
    | value NEQ value                               { cout << "!=" << endl; }
    | value LE value                                { cout << "<" << endl; }
    | value GE value                                { cout << ">" << endl; }
    | value LEQ value                               { cout << "<=" << endl; }
    | value GEQ value                               { cout << ">=" << endl; }
    ;

value:
    NUM                                         { cout << "num" << endl; }
    | identifier
    ;

identifier:
    IDENTIFIER                                     { cout << "ide" << endl; }
    | IDENTIFIER LB IDENTIFIER RB               { cout << "ide ( ide )" << endl; }
    | IDENTIFIER LB NUM RB                   { cout << "ide ( num )" << endl; }
    ;

%%
 void yyerror (const string str) {
   cout << "Error" << str << endl;
 }
 
int main(int argv, char* argc[]) {
    if (argv != 3) {
    	cout << "Error: inaccurate number of arguments. \e[1m[ VALID USAGE ]\e[0m compiler input output" << endl;
   	exit(1);
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL) {
    	cout << (argc[1], 0, "File does not exist:");
    	exit(1);
    } else {
    yyparse();
    //printToFile(argc[2]);
    cout << "\nCompiled without errors \n" << endl;
    }
	return 0;
}



