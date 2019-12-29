%{
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

typedef struct {
	string name;
   string type; //NUM, IDE, ARR
   int initialized;
   int counter;
   long long int begin;
	long long int mem;
	long long int local;
  	long long int tableSize;
} Identifier;

vector<string> codeStack;
map<string, Identifier> identifierStack;


int memCounter;
int assignFlag;
int writeFlag;
Identifier assignTarget;
string tabAssignTargetIndex = "-1";
string expressionArguments[2] = {"-1", "-1"};
string argumentsTabIndex[2] = {"-1", "-1"};

//BISON
void ident(string variable, int yylineo);
void identIdent(string tab, string i, int yylineo);
void identNum(string tab, string num, int yylineo);
void declareIdent(string variable, int yylineno);
void declareArr(string variable, string begin, string end, int yylineno);
void number(string variable, int yylineno);
void soloValue();

//HELPERS
void createIdentifier(Identifier *s, string name, long long int isLocal, long long int isArray, string type, long long int begin);
void insertIdentifier(string key, Identifier i);
void removeIdentifier(string key);
void setUp();
string decToBin(long long int n);

//MACHINE
void registerToMem(long long int mem);
void memToRegister(long long int mem);
void setRegister(string number);
void zeroRegister();
void pushCommand(string str);
void pushCommandOneArg(string str, long long int num);

extern int yylex();
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
    identifier ASSIGN  {
    	assignFlag = 0;
    }  expression SEM {
   	 if(assignTarget.type == "ARR") {

      	Identifier index = identifierStack.at(tabAssignTargetIndex);
      		if(index.type == "NUM") {
      			 if(stoi(index.name) < assignTarget.begin || stoi(index.name) >= assignTarget.begin + assignTarget.tableSize) {
      			 	cout << "Błąd [okolice linii " << yylineno << \
            		"]: Element z poza tablicy" << assignTarget.name<<"[" << stoi(index.name) << "]" << endl;
      				exit(1);
      			 }
                long long int tabElMem = assignTarget.mem + (stoi(index.name) - assignTarget.begin);
                registerToMem(tabElMem);
                removeIdentifier(index.name);
            }
            else {
             	 registerToMem(3);
                setRegister(to_string(assignTarget.begin));
                pushCommandOneArg("SUB", index.mem);
                registerToMem(4);
                setRegister(to_string(assignTarget.mem));
                pushCommandOneArg("SUB", 4);
                registerToMem(4);
                memToRegister(3);
                pushCommandOneArg("STOREI", 4);
            }
        }
        else if(assignTarget.local == 0) {
            registerToMem(assignTarget.mem);
        }
      	else {
            cout << "Błąd [okolice linii " << yylineno << \
            "]: Próba modyfikacji iteratora pętli." << endl;
      		exit(1);
      	}
      	
      identifierStack.at(assignTarget.name).initialized = 1;
      assignFlag = 1;
    }
    | IF condition THEN commands ELSE commands ENDIF { cout << "else" << endl; }  
    | IF condition THEN commands ENDIF { cout << "if" << endl; }            
    | WHILE condition DO commands ENDWHILE { cout << "while" << endl; }
    | DO condition WHILE commands ENDDO         { cout << "do" << endl; }
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR { cout << "for" << endl; }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR { cout << "downfor" << endl; }
    | READ identifier SEM                         { cout << "read" << endl; }
    | WRITE {
      	writeFlag = 0;
			assignFlag = 1;
		} value SEM                                  { cout << "write" << endl; }
    ;

expression:
    value                                          { soloValue(); }
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
    NUM                                         { number($1, yylineno); }
    | identifier
    ;

identifier:
    IDENTIFIER                                     { ident($1, yylineno); }
    | IDENTIFIER LB IDENTIFIER RB               { identIdent($1,$3, yylineno); }
    | IDENTIFIER LB NUM RB                   { identNum($1,$3, yylineno); }
    ;

%%


void setUp() {

    	writeFlag = 0;
		assignFlag = 1;
		memCounter = 5;
		
		//create 0
		pushCommand("# 0 to p1");
		pushCommandOneArg("SUB", 0);
		registerToMem(1);
		
		//create 1
		pushCommand("# 1 to p2");
		pushCommand("INC");
		registerToMem(2);
		pushCommand("#SET UP END");
}

void registerToMem(long long int mem) {
	pushCommandOneArg("STORE", mem);
}

void memToRegister(long long int mem) {
	pushCommandOneArg("LOAD", mem);
	/*registerValue = -1;*/
}

void pushCommand(string str) {
    cout << str << endl;
    codeStack.push_back(str);
}

void pushCommandOneArg(string str, long long int num) {
    string temp = str + " " + to_string(num);
    cout << temp << endl;
    codeStack.push_back(temp);
}

void setRegister(string number) {
    long long int n = stoll(number);
    string bin = decToBin(n);
	long long int limit = bin.size();
    zeroRegister();
	for(long long int i = 0; i < limit; ++i){
		if(bin[i] == '1'){
			if(n > 0)
			pushCommand("INC");
			else
			pushCommand("DEC");
			/*registerValue++;*/
		}
		if(i < (limit - 1)){
	        pushCommand("SHL");
	        /*registerValue *= 2;*/
		}
	}
}

void zeroRegister() {
	/*if(registerValue != 0){*/
		memToRegister(1);
		/*registerValue = 0;*/
	/*}*/
}

string decToBin(long long int n) {
    string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    return r;
}

void soloValue() {
	Identifier value = identifierStack.at(expressionArguments[0]);
        if(value.type == "NUM") {
            setRegister(value.name);
            removeIdentifier(value.name);
        }
        else if(value.type == "IDE") {
            memToRegister(value.mem);
        }
        else {
            Identifier index = identifierStack.at(argumentsTabIndex[0]);
            if(index.type == "NUM") {

                long long int tabElMem = value.mem + (stoi(index.name) - assignTarget.begin);
                memToRegister(tabElMem);
                removeIdentifier(index.name);
            }
            else {
            	 setRegister(to_string(value.begin));
                pushCommandOneArg("SUB", index.mem);
                registerToMem(3);
                setRegister(to_string(value.mem));
                pushCommandOneArg("SUB", 3);
                pushCommandOneArg("LOADI", 0);             
            }
        }
        
   if (!writeFlag) {
   	expressionArguments[0] = "-1";
  		argumentsTabIndex[0] = "-1";
   }
}

void number(string variable, int yylineno) {
	if (assignFlag) {
   	cout << "Błąd [okolice linii " << yylineno << \
   	  "]: Próba przypisania do stałej." << endl;
           	exit(1);
    }
    
    Identifier s;
    createIdentifier(&s, variable, 0, 0, "NUM",0);
    insertIdentifier(variable, s);
    if (expressionArguments[0] == "-1"){
    	expressionArguments[0] = variable;
    } else {
    	expressionArguments[1] = variable;
    }
}

void declareIdent(string variable, int yylineno) {
        if(identifierStack.find(variable)!=identifierStack.end()) {
            cout << "Błąd [okolice linii " << yylineno \
            << "]: Kolejna deklaracja zmiennej " << variable << endl;
            exit(1);
        }
        else {
            Identifier s;
            createIdentifier(&s, variable, 0, 0, "IDE", 0);
            insertIdentifier(variable, s);
        }
}

void declareArr(string variable, string begin, string end, int yylineno) {
       
       if(identifierStack.find(variable)!=identifierStack.end()) {
            cout << "Błąd [okolice linii " << yylineno \
            << "]: Kolejna deklaracja zmiennej " << variable<< endl;
            exit(1);
        }
        else if (stoi(begin) > stoi(end)) {
            cout << "Błąd [okolice linii " << yylineno \
            << "]: Deklarowanie tablicy" << variable << "o końcowym index'ie mniejszym niż początkowym" << endl;
            exit(1);
        }
        else {
            long long int size = stoi(end) - stoi(begin) + 1; 
            Identifier s;
            createIdentifier(&s, variable, 0, size, "ARR", stoi(begin));
            insertIdentifier(variable, s);
            memCounter += size-1; // może size - 1, bo przy insertIdentifire dodawana jest juz 1
            //setRegister(to_string(s.mem+1));	// do p0 wprowadz numer rejestru w ktorym zaczyna sie tab
            //registerToMem(s.mem); po co przy deklaracji zapisywać w komorce jej adres				
        }
}


void ident(string variable, int yylineo) {

        if(identifierStack.find(variable) == identifierStack.end()) {
            cout << "Line: " << yylineno << ". Error: Undeclared variable " << variable << endl;
            exit(1);
        }
        
        if(identifierStack.at(variable).tableSize == 0) {
            if(!assignFlag) {
                if(identifierStack.at(variable).initialized == 0) {
                    cout << "Błąd [okolice linii " << yylineno << \
                    "]: Próba użycia niezainicjalizowanej zmiennej " << variable << "." << endl;
                    exit(1);
                }
                if (expressionArguments[0] == "-1"){
                    expressionArguments[0] = variable;
                }
                else{
                    expressionArguments[1] = variable;
                }

            }
            else {
                assignTarget = identifierStack.at(variable);
            }
        }
        else {
          cout << "Błąd [okolice linii " << yylineno << \
          "]: Brak odwołania do elementu tablicy " << variable << "." << endl;
          exit(1);
        }
}

void identIdent(string tab, string i, int yylineo) {
		if(identifierStack.find(tab) == identifierStack.end()) {
            cout << "Błąd [okolice linii " << yylineno << \
            "]: Zmienna " << tab << " nie została zadeklarowana." << endl;
            exit(1);
        }
        
        if(identifierStack.find(i) == identifierStack.end()) {
            cout << "Błąd [okolice linii " << yylineno << \
            "]: Zmienna " << i << " nie została zadeklarowana." << endl;
            exit(1);
        }

        if(identifierStack.at(tab).tableSize == 0) {
            cout << "Błąd [okolice linii " << yylineno << \
            "]: Zmienna " << tab << " nie jest tablicą." << endl;
            exit(1);
        }
        
        if(identifierStack.at(i).initialized == 0) {
             cout << "Błąd [okolice linii " << yylineno << \
            "]: Próba użycia niezainicjalizowanej zmiennej " << i << "." << endl;
             exit(1);
       }

       if(!assignFlag) {
                //TODO czy wywalać błąd niezainicjalizowanej
                //zmiennej dla elementu tablicy-> raczej nie ma czasu ;/
         if (expressionArguments[0] == "-1"){
            	expressionArguments[0] = tab;
               argumentsTabIndex[0] = i;
         }
         else {
         	expressionArguments[1] = tab;
            argumentsTabIndex[1] = i;
         }
		} else {
                assignTarget = identifierStack.at(tab);
                tabAssignTargetIndex = i;
      }
}

void identNum(string tab, string num, int yylineo) {
	if(identifierStack.find(tab) == identifierStack.end()) {
            cout << "Błąd [okolice linii " << yylineno << \
            "]: Zmienna " << tab << " nie została zadeklarowana." << endl;
            exit(1);
        }

        if(identifierStack.at(tab).tableSize == 0) {
            cout << "Błąd [okolice linii " << yylineno << \
            "]: Zmienna " << tab << " nie jest tablicą." << endl;
            exit(1);
        }
        
        else {
            Identifier s;
            createIdentifier(&s, num, 0, 0, "NUM", 0);
            insertIdentifier(num, s);

            if(!assignFlag){
                //TODO czy wywalać błąd niezainicjalizowanej
                //zmiennej dla elementu tablicy
                if (expressionArguments[0] == "-1"){
                    expressionArguments[0] = tab;
                    argumentsTabIndex[0] = num;
                }
                else{
                    expressionArguments[1] = tab;
                    argumentsTabIndex[1] = num;
                }

            }
            else {
                assignTarget = identifierStack.at(tab);
                tabAssignTargetIndex = num;
            }
        }
}

void insertIdentifier(string key, Identifier i) {
    if(identifierStack.count(key) == 0) {
        identifierStack.insert(make_pair(key, i));
        identifierStack.at(key).counter = 0;
        memCounter++;	// czy potrzebne jest dodawanie 1 gdy dodaje sie tablice?
    }
    else {
        identifierStack.at(key).counter = identifierStack.at(key).counter+1;
    }
    /*cout << "Add: " << key << " " << memCounter-1 << endl;*/
}

void createIdentifier(Identifier *s, string name, long long int isLocal,
    long long int isArray, string type, long long int begin){
    s->name = name;
    s->mem = memCounter;	//ktore komorki zajmuje zmienna, gdzie sie zaczyna
    s->type = type;
    s->initialized = 0;
    
    if(isLocal){
    	s->local = 1;
    }else{
    	s->local = 0;
    }
    
    if(isArray){
      s->tableSize = isArray;
    }else{
      s->tableSize = 0;
    }
    
    s->begin = begin;
}

void removeIdentifier(string key) {
    if(identifierStack.count(key) > 0) {
        if(identifierStack.at(key).counter > 0) {
            identifierStack.at(key).counter = identifierStack.at(key).counter-1;
        }
        else {
            identifierStack.erase(key);
            memCounter--;
        }
    }
    //cout << "Remove: " << key << endl;
}



 void yyerror (const string str) {
   cout << "Line: " << yylineno <<". Error: " << str << endl;
   exit(1);
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
    setUp();
    yyparse();
    //printToFile(argc[2]);
    cout << "\n#Compiled without errors \n" << endl;
    }
	return 0;
}



