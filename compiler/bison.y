%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
void yyerror(const string str);
%}

%define parse.error verbose
%define parse.lac full
%union {
    char* str;
}

%token <str> NUM IDENTIFIER
%token <str> DECLARE BEG END READ WRITE FOR FROM TO DOWNTO DO WHILE IF THEN ELSE ENDFOR ENDWHILE ENDIF ENDDO
%token <str> ASSIGN NEQ EQ LE GE LEQ GEQ SEM COMMA COL ADD SUB MUL DIV MOD LB RB

%type <str> value
%type <str> identifier

%%
program:
    DECLARE declarations BEG commands END { pushCommand("HALT"); }
    | BEG commands END  { pushCommand("HALT"); }
    ;

declarations:
    declarations COMMA IDENTIFIER { declareIdent($3, yylineno);}
    | declarations COMMA IDENTIFIER LB NUM COL NUM RB {  declareArr($3,$5,$7,yylineno); }
    | IDENTIFIER { declareIdent($1, yylineno); }
    | IDENTIFIER LB NUM COL NUM RB { declareArr($1,$3,$5,yylineno); }
    ;

commands:
    commands command
    | command
    ;

command:
	 identifier ASSIGN  { assignFlag = 0; }  expression SEM { assign(yylineno); }
    | IF { assignFlag = 0; depth++; } condition {assignFlag = 1;} THEN commands ifbody
    | while condition { assignFlag = 1; } DO commands ENDWHILE { whileDo(); }
    | DO { assignFlag = 1; whileBegin(); } commands while condition ENDDO { doWhile(); }  
    | FOR IDENTIFIER { forBegin($2, yylineno); } FROM value forbody
    | READ  { assignFlag = 1; } identifier  SEM { read(yylineno); }
    | WRITE { writeFlag = 1; assignFlag = 0; } value SEM { soloValue(); }
    ;

ifbody:
	ELSE { elseBlock(); } commands ENDIF { elseEndIfBlock(); }
	| ENDIF { endIfBlock(); }
	;

while: 
	WHILE { assignFlag = 0; whileBegin(); }
	; 

forbody:
	TO value DO { forTo(); } commands ENDFOR { forEnd(); }
	| DOWNTO value DO { forDownTo(); } commands ENDFOR { forEnd(); }
	;
	
expression:
    value                                          { soloValue(); }
    | value ADD value                              { add(); }
    | value SUB value                              { subtract(); }
    | value MUL value                              { multiply(); }
    | value DIV value                              { divide(); }
    | value MOD value                              { modulo(); }
    ;

condition:
    value EQ value                                 { ifEqual(); }
    | value NEQ value                              { ifNotEqual(); }
    | value LE value                               { ifLess(); }
    | value GE value                               { ifGreater(); }
    | value LEQ value                              { ifLessEqual(); }
    | value GEQ value                              { ifGreaterEqual(); }
    ;

value:
    NUM                                            { number($1, yylineno); }
    | identifier
    ;

identifier:
    IDENTIFIER                                     { ident($1, yylineno); }
    | IDENTIFIER LB IDENTIFIER RB               	{ identIdent($1,$3, yylineno); }
    | IDENTIFIER LB NUM RB                   		{ identNum($1,$3, yylineno); }
    ;

%%

void yyerror(const string str) {
  cout << "Line: " << yylineno << ". Error: Unrecognized symbol" << endl;
  exit(1);
}

int main(int argv, char * argc[]) {
  if (argv != 3) {
    cout << "Error: inaccurate number of arguments. \e[1m[ VALID USAGE ]\e[0m compiler input output" << endl;
    exit(1);
  }

  yyin = fopen(argc[1], "r");
  if (yyin == NULL) {
    cout << (argc[1], 0, "File does not exist");
    exit(1);
  } else {
    setUp();
    yyparse();
    string file = argc[2];
    printCode(file);
    cout << "Compiled without errors" << endl;
  }
  return 0;
}
