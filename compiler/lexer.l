%{
#include <iostream>
#include <string>
#include <vector>
#include "bison.tab.h"
%}

%x COMMENT
%option noyywrap
%option yylineno
%%

"["             		BEGIN(COMMENT);
<COMMENT>"]"         BEGIN(INITIAL);
<COMMENT>(\n|.)		;
<COMMENT><<EOF>>   	{printf("Error: unclosed comment.\n"); yyterminate();}

"DECLARE"				return DECLARE;
"BEGIN"		    		return BEG;
"END"		    			return END;

"ASSIGN"		    		return ASSIGN;

"IF"		    			return IF;
"THEN"		    		return THEN;
"ELSE"		    		return ELSE;
"ENDIF"		    		return ENDIF;

"WHILE"		    		return WHILE;
"DO"		    			return DO;
"ENDWHILE"	    		return ENDWHILE;
"ENDDO"	        		return ENDDO;

"FOR"		    			return FOR;
"FROM"		    		return FROM;
"TO"		    			return TO;
"DOWNTO"	    			return DOWNTO;
"ENDFOR"	    			return ENDFOR;

"READ"		    		return READ;
"WRITE"		    		return WRITE;

"PLUS"             	return ADD;
"MINUS"             	return SUB;
"TIMES"             	return MUL;
"DIV"             	return DIV;
"MOD"             	return MOD;
"("             		return LB;
")"             		return RB;

"EQ"             		return EQ;
"NEQ"             	return NEQ;
"LE"             		return LE;
"GE"            		return GE;
"LEQ"            		return LEQ;
"GEQ"            		return GEQ;



-?[0-9]+          { yylval.str = (char *)strdup(yytext); return NUM; }
[_a-z]+         { yylval.str = (char *)strdup(yytext); return IDENTIFIER; }

";"            return SEM;
":"            return COL;
","				return COMMA;	

[ \t\r\n]+		;

.              { printf("line: %d. Error: Unknown symbol.\n",yylineno); yyterminate(); }
