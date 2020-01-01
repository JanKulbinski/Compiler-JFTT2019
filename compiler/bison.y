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
string tabAssignTargetIndex = "null";
string expressionArguments[2] = {"null", "null"};
string argumentsTabIndex[2] = {"null", "null"};

//BISON
void ident(string variable, int yylineo);
void identIdent(string tab, string i, int yylineo);
void identNum(string tab, string num, int yylineo);
void declareIdent(string variable, int yylineno);
void declareArr(string variable, string begin, string end, int yylineno);
void number(string variable, int yylineno);
void soloValue();
void assign();
void read();
void add();
void subtract();
void multiplication();

//HELPERS
void createIdentifier(Identifier *s, string name, long long int isLocal, long long int isArray, string type, long long int begin);
void insertIdentifier(string key, Identifier i);
void removeIdentifier(string key);
void setUp();
string decToBin(long long int n);
void printCode(string outFileName);
long long int setToTempMem(Identifier a, Identifier aI, long long int tempMem, int isJZERO, int isRemoval);
int isPowerOf2();

//MACHINE
void registerToMem(long long int mem);
void memToRegister(long long int mem);
void setRegister(string number);
void zeroRegister();
void pushCommand(string str);
void pushCommandOneArg(string str, long long int num);
void setToRegister(Identifier a, Identifier aI);

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
    identifier ASSIGN  { assignFlag = 0; }  expression SEM { assign(); }
    | IF condition THEN commands ELSE commands ENDIF { cout << "else" << endl; }  
    | IF condition THEN commands ENDIF { cout << "if" << endl; }            
    | WHILE condition DO commands ENDWHILE { cout << "while" << endl; }
    | DO condition WHILE commands ENDDO         { cout << "do" << endl; }
    | FOR identifier FROM value TO value DO commands ENDFOR { cout << "for" << endl; }
    | FOR identifier FROM value DOWNTO value DO commands ENDFOR { cout << "downfor" << endl; }
    | READ  { assignFlag = 1; } identifier  SEM { read(); }
    | WRITE { writeFlag = 1; assignFlag = 0; } value SEM { soloValue(); }
    ;

expression:
    value                                          { soloValue(); }
    | value ADD value                              { add(); }
    | value SUB value                              { subtract(); }
    | value MUL value                              { multiplication(); }
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
		memCounter = 10;
		
		//create 0
	//	pushCommand("# 0 to p1");
		pushCommandOneArg("SUB", 0);
		registerToMem(1);
		
		//create 1
	//	pushCommand("# 1 to p2");
		pushCommand("INC");
		registerToMem(2);
		
		//create -1
	//	pushCommand("# 1 to p9");
		pushCommand("DEC");
		pushCommand("DEC");
		registerToMem(9);
	//	pushCommand("#SET UP END");
}

void registerToMem(long long int mem) {
	pushCommandOneArg("STORE", mem);
}

void memToRegister(long long int mem) {
	pushCommandOneArg("LOAD", mem);
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

int isPowerOf2(string value) {
	string bin = decToBin(stoi(value));
	int length = bin.size();
	if (bin[0] == '1' && bin.substr(1).find("1") == string::npos)
    	return length;
   else
   	return 0;
}

void multiplication() {
		  Identifier a = identifierStack.at(expressionArguments[0]);
        Identifier b = identifierStack.at(expressionArguments[1]);
        
        Identifier aI, bI;
        if(identifierStack.count(argumentsTabIndex[0]) > 0)
            aI = identifierStack.at(argumentsTabIndex[0]);
        if(identifierStack.count(argumentsTabIndex[1]) > 0)
            bI = identifierStack.at(argumentsTabIndex[1]);
            
         if(a.type == "NUM" && b.type == "NUM") {
            long long int val = stoi(a.name) * stoi(b.name);
            setRegister(to_string(val));
            removeIdentifier(a.name);
            removeIdentifier(b.name);
            
        } else if(a.type == "NUM" && isPowerOf2(a.name)) {
        		setRegister(to_string(isPowerOf2(a.name) - 1));
        		registerToMem(3);
        		setToRegister(b, bI);
     			
     			if(stoi(a.name) < 0) {
     				pushCommandOneArg("SHIFT", 3);
     				pushCommandOneArg("SHIFT", 2);
     				registerToMem(4);
     				setToRegister(b,bI);
     				pushCommandOneArg("SHIFT", 3);
     				pushCommandOneArg("SUB", 4);
     				
     			} else if(stoi(a.name) > 0) {
     				pushCommandOneArg("SHIFT", 3);
     			} else {
     				zeroRegister();
     			}
        		removeIdentifier(a.name);
        		
        } else if(b.type == "NUM" && isPowerOf2(b.name)) {
        		setRegister(to_string(isPowerOf2(b.name) - 1));
        		registerToMem(3);
        		setToRegister(a, aI);
     			
     			if(stoi(b.name) < 0) {
     				pushCommandOneArg("SHIFT", 3);
     				pushCommandOneArg("SHIFT", 2);
     				registerToMem(4);
     				setToRegister(a,aI);
     				pushCommandOneArg("SHIFT", 3);
     				pushCommandOneArg("SUB", 4);
     				
     			} else if(stoi(b.name) > 0) {
     				pushCommandOneArg("SHIFT", 3);
     			} else {
     				zeroRegister();
     			}
        		removeIdentifier(b.name);
        		
        } else {  
            /* Optimalization plans: change order a,b if a > b
             if(a.type != "ARR" && b.type != "ARR")
                sub(b, a, 0, 0);
            else
                subTab(b, a, bI, aI, 0, 0);
            long long int stackJ = codeStack.size();
            pushCommand("JZERO");
            setToTempMem(b, bI, 6, 0, 0);
            setToTempMem(a, aI, 5, 0, 0);
            pushCommand("JUMP");
            addInt(stackJ, codeStack.size());
            stackJ = codeStack.size()-1; */
       
				zeroRegister();
				registerToMem(7);
				pushCommand("DEC");
				registerToMem(8);
            setToTempMem(a, aI, 5, 0, 1);
            setToTempMem(b, bI, 6, 0, 1);
            
				//if (b < 0) b = -b         	
            pushCommandOneArg("JPOS", codeStack.size()+8);
            pushCommandOneArg("SHIFT", 2);
            registerToMem(3);
            memToRegister(6);
            pushCommandOneArg("SUB", 3);
            registerToMem(6); 
           	zeroRegister();
           	registerToMem(8);
           	
           	//if (a < 0) a = -a
            memToRegister(5);
            pushCommandOneArg("JPOS", codeStack.size()+10);
            pushCommandOneArg("SHIFT", 2);
            registerToMem(3);
            memToRegister(5);
            pushCommandOneArg("SUB", 3);
            registerToMem(5);
            memToRegister(8);
            pushCommand("INC");
            registerToMem(8);
            memToRegister(5);
            
            // a * b
            int stackJ = codeStack.size();
            pushCommandOneArg("JZERO", codeStack.size()+16);
            pushCommandOneArg("SHIFT", 9);
            pushCommandOneArg("SHIFT", 2);
            pushCommandOneArg("SUB", 5);
            pushCommandOneArg("JNEG", codeStack.size()+2);
            pushCommandOneArg("JUMP", codeStack.size()+4);
            memToRegister(7);
            pushCommandOneArg("ADD", 6);
            registerToMem(7);
            memToRegister(6);
            pushCommandOneArg("SHIFT", 2);
            registerToMem(6);
            memToRegister(5);
            pushCommandOneArg("SHIFT", 9);
            registerToMem(5);
            pushCommandOneArg("JUMP", stackJ);
            
            //if(a.old * b.old < 0) c = -c 
            memToRegister(8);
            pushCommandOneArg("JNEG", codeStack.size()+8);
            pushCommandOneArg("JPOS", codeStack.size()+7);     
            memToRegister(7);
            pushCommandOneArg("SHIFT", 2);
            registerToMem(3);
            memToRegister(7);
            pushCommandOneArg("SUB", 3);
            pushCommandOneArg("JUMP", codeStack.size()+2);
				memToRegister(7);
		}	
    argumentsTabIndex[0] = "null";
    argumentsTabIndex[1] = "null";
    expressionArguments[0] = "null";
    expressionArguments[1] = "null";
}

void setToRegister(Identifier a, Identifier aI) { 
	if(a.type == "NUM") {
   	setRegister(a.name);
   } else if(a.type == "IDE") {
      memToRegister(a.mem);
   } else if(a.type == "ARR" && aI.type == "NUM") {
        long long int addr = a.mem + (stoi(aI.name) - a.begin);
        memToRegister(addr);
   } else if(a.type == "ARR" && aI.type == "IDE") {
            setRegister(to_string(a.mem - a.begin));
            pushCommandOneArg("ADD", aI.mem);
            pushCommandOneArg("LOADI", 0);
    }
}

long long int setToTempMem(Identifier a, Identifier aI, long long int tempMem, int isJZERO, int isRemoval) {
    long long int mem = 0;
    if(a.type == "NUM") {
        setRegister(a.name);
        if(isJZERO) {
            mem = codeStack.size();
            pushCommand("JZERO");
        }
        registerToMem(tempMem);
        if(isRemoval)
            removeIdentifier(a.name);
    }
    else if(a.type == "IDE") {
        memToRegister(a.mem);
        if(isJZERO) {
            mem = codeStack.size();
            pushCommand("JZERO"); //JZERO END
        }
        registerToMem(tempMem);
    }
    else if(a.type == "ARR" && aI.type == "NUM") {
        long long int addr = a.mem + (stoi(aI.name) - a.begin);
        memToRegister(addr);
        if(isJZERO) {
            mem = codeStack.size();
            pushCommand("JZERO"); //JZERO END
        }
        registerToMem(tempMem);
        if(isRemoval)
            removeIdentifier(aI.name);
    }
    else if(a.type == "ARR" && aI.type == "IDE") {
            setRegister(to_string(a.mem - a.begin));
            pushCommandOneArg("ADD", aI.mem);
            pushCommandOneArg("LOADI", 0);
        if(isJZERO) {
            mem = codeStack.size();
            pushCommand("JZERO"); //JZERO END
        }
        registerToMem(tempMem);
    }
    return mem;
}


void setRegister(string number) {
   long long int n = stoll(number);
   string bin = decToBin(n);
	long long int limit = bin.size();
   zeroRegister();
    
   if(limit <= 4) {
   	for(int i = 0; i < abs(n); i++) {
   		if(n < 0)
         	pushCommand("DEC");
         else
         	pushCommand("INC");
      }       
   } else {
		for(long long int i = 0; i < limit; ++i){
			if(bin[i] == '1'){
				if(n > 0)
					pushCommand("INC");
				else
					pushCommand("DEC");
			}
			if(i < (limit - 1)){
	        pushCommandOneArg("SHIFT", 2);
			}
		}
	}
}

void zeroRegister() {
		memToRegister(1);
}

string decToBin(long long int n) {
    string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    return r;
}

void subtract() {       
  Identifier a = identifierStack.at(expressionArguments[0]);
  Identifier b = identifierStack.at(expressionArguments[1]);     
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
  
  if(a.type != "ARR" && b.type != "ARR") {
  	if(a.type == "NUM" && b.type == "NUM") {
        long long int val = stoi(a.name) - stoi(b.name);
        setRegister(to_string(val));
        removeIdentifier(a.name);
        removeIdentifier(b.name);
    } else if(a.type == "NUM" && b.type == "IDE") {
            setRegister(a.name);
            pushCommandOneArg("SUB", b.mem);
            removeIdentifier(a.name);
    } else if(a.type == "IDE" && b.type == "NUM") {
            setRegister(b.name);
            registerToMem(3);
            memToRegister(a.mem);
            pushCommandOneArg("SUB", 3);
            removeIdentifier(b.name);
    } else if(a.type == "IDE" && b.type == "IDE") {
        if(a.name == b.name) {
				zeroRegister();
        } else {
           memToRegister(a.mem);
           pushCommandOneArg("SUB", b.mem);
        }
   } 
  } else {
   Identifier aIndex, bIndex;
   if(identifierStack.count(argumentsTabIndex[0]) > 0)		
  		aIndex = identifierStack.at(argumentsTabIndex[0]);
   if(identifierStack.count(argumentsTabIndex[1]) > 0)
  		bIndex = identifierStack.at(argumentsTabIndex[1]);
  	argumentsTabIndex[0] = "null";
  	argumentsTabIndex[1] = "null";	
  
  if(a.type == "NUM" && b.type == "ARR") {
        if(bIndex.type == "NUM") {
            long long int addr = b.mem + (stoi(bIndex.name) - b.begin);
            setRegister(a.name);
            pushCommandOneArg("SUB", addr);
            removeIdentifier(a.name);
            removeIdentifier(bIndex.name);
            
        } else if(bIndex.type == "IDE") {
            setRegister(to_string(b.mem - b.begin));
            pushCommandOneArg("ADD", bIndex.mem);
            pushCommandOneArg("LOADI", 0);
            registerToMem(3);
            setRegister(a.name);
            pushCommandOneArg("SUB", 3);
            removeIdentifier(a.name);
        }
    } else if(a.type == "ARR" && b.type == "NUM") {
     		if(aIndex.type == "NUM") {
            long long int addr = a.mem + (stoi(aIndex.name) - a.begin);
            setRegister(b.name);
            registerToMem(3);
            memToRegister(addr);
            pushCommandOneArg("SUB", 3);
            removeIdentifier(b.name);
            removeIdentifier(aIndex.name);
            
        } else if(aIndex.type == "IDE") {
            setRegister(b.name);
            registerToMem(3);
            setRegister(to_string((a.mem - a.begin)));
            pushCommandOneArg("ADD", aIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("SUB", 3);
            removeIdentifier(b.name);
        }
    } else if(a.type == "IDE" && b.type == "ARR") { 
        if(bIndex.type == "NUM") {
        		long long int addr = b.mem + (stoi(bIndex.name) - b.begin);
            memToRegister(a.mem);
            pushCommandOneArg("SUB", addr);
            removeIdentifier(bIndex.name);
        }
        else if(bIndex.type == "IDE") {
            setRegister(to_string(b.mem - b.begin));
            pushCommandOneArg("ADD", bIndex.mem);
            pushCommandOneArg("LOADI", 0);
            registerToMem(3);
            memToRegister(a.mem);
            pushCommandOneArg("SUB", 3);
        }
    } else if(a.type == "ARR" && b.type == "IDE") { 
        if(aIndex.type == "NUM") {
        		long long int addr = a.mem + (stoi(aIndex.name) - a.begin);
        		memToRegister(addr);
            pushCommandOneArg("SUB", b.mem);
            removeIdentifier(aIndex.name);
        }
        else if(aIndex.type == "IDE") {
            setRegister(to_string(a.mem - a.begin));
            pushCommandOneArg("ADD", aIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("SUB", b.mem);
        }
    } else if(a.type == "ARR" && b.type == "ARR") { 
        if(aIndex.type == "NUM" && bIndex.type == "NUM") {
            long long int addrA = a.mem + (stoi(aIndex.name) - a.begin);
            long long int addrB = b.mem + (stoi(bIndex.name) - b.begin);
            
            if(a.name == b.name && addrA == addrB)
                zeroRegister();
            else {
            	 memToRegister(addrA);
                pushCommandOneArg("SUB", addrB);
            }
            removeIdentifier(aIndex.name);
            removeIdentifier(bIndex.name);
        }
        else if(aIndex.type == "NUM" && bIndex.type == "IDE") { 
            long long int addrA = a.mem + (stoi(aIndex.name) - a.begin);  
            setRegister(to_string((b.mem - b.begin)));
            pushCommandOneArg("ADD", bIndex.mem);
            pushCommandOneArg("LOADI", 0);
            registerToMem(3);
            memToRegister(addrA);
            pushCommandOneArg("SUB", 3);
            removeIdentifier(aIndex.name);
        }
        else if(aIndex.type == "IDE" && bIndex.type == "NUM") { 
            long long int addrB = b.mem + (stoi(bIndex.name) - b.begin);  
            setRegister(to_string((a.mem - a.begin)));
            pushCommandOneArg("ADD", aIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("SUB", addrB);
            removeIdentifier(bIndex.name);
        }
        else if(aIndex.type == "IDE" && bIndex.type == "IDE") { 
            if(a.name == b.name && aIndex.name == bIndex.name)
                zeroRegister();
            else {
                setRegister(to_string((b.mem - b.begin)));
            	 pushCommandOneArg("ADD", bIndex.mem);
            	 pushCommandOneArg("LOADI", 0);
            	 registerToMem(3); 
            	 setRegister(to_string((a.mem - a.begin)));
            	 pushCommandOneArg("ADD", aIndex.mem);
            	 pushCommandOneArg("LOADI", 0);
            	 pushCommandOneArg("SUB", 3);
            }
        }
   } 
} 		
}

void add() {        
  Identifier a = identifierStack.at(expressionArguments[0]);
  Identifier b = identifierStack.at(expressionArguments[1]);     
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
  
  if(a.type != "ARR" && b.type != "ARR") {
  	if(a.type == "NUM" && b.type == "NUM") {
        long long int val = stoi(a.name) + stoi(b.name);
        setRegister(to_string(val));
        removeIdentifier(a.name);
        removeIdentifier(b.name);
    } else if(a.type == "NUM" && b.type == "IDE") {
            setRegister(a.name);
            pushCommandOneArg("ADD", b.mem);
            removeIdentifier(a.name);
    } else if(a.type == "IDE" && b.type == "NUM") {
            setRegister(b.name);
            pushCommandOneArg("ADD", a.mem);
            removeIdentifier(b.name);
    } else if(a.type == "IDE" && b.type == "IDE") {
        if(a.name == b.name) {
            memToRegister(a.mem);
            pushCommand("SHL");
        } else {
           memToRegister(a.mem);
           pushCommandOneArg("ADD", b.mem);
        }
   } 
  } else {
   Identifier aIndex, bIndex;
   if(identifierStack.count(argumentsTabIndex[0]) > 0)		
  		aIndex = identifierStack.at(argumentsTabIndex[0]);
   if(identifierStack.count(argumentsTabIndex[1]) > 0)
  		bIndex = identifierStack.at(argumentsTabIndex[1]);
  	argumentsTabIndex[0] = "null";
  	argumentsTabIndex[1] = "null";	
  
  if(a.type == "NUM" && b.type == "ARR") {
        if(bIndex.type == "NUM") {
            long long int addr = b.mem + (stoi(bIndex.name) - b.begin);
            setRegister(a.name);
            pushCommandOneArg("ADD", addr);
            removeIdentifier(a.name);
            removeIdentifier(bIndex.name);
            
        } else if(bIndex.type == "IDE") {
            setRegister(to_string(b.mem - b.begin));
            pushCommandOneArg("ADD", bIndex.mem);
            pushCommandOneArg("LOADI", 0);
            registerToMem(3);
            setRegister(a.name);
            pushCommandOneArg("ADD", 3);
            removeIdentifier(a.name);
        }
    } else if(a.type == "ARR" && b.type == "NUM") { 
     		if(aIndex.type == "NUM") {
            long long int addr = a.mem + (stoi(aIndex.name) - a.begin);
            setRegister(b.name);
            pushCommandOneArg("ADD", addr);
            removeIdentifier(b.name);
            removeIdentifier(aIndex.name);
            
        } else if(aIndex.type == "IDE") {
            setRegister(to_string((a.mem - a.begin)));
            pushCommandOneArg("ADD", aIndex.mem);
            pushCommandOneArg("LOADI", 0);
            registerToMem(3);
            setRegister(b.name);
            pushCommandOneArg("ADD", 3);
            removeIdentifier(b.name);
        }
    } else if(a.type == "IDE" && b.type == "ARR") { 
        if(bIndex.type == "NUM") {
        		long long int addr = b.mem + (stoi(bIndex.name) - b.begin);
        		memToRegister(a.mem);
            pushCommandOneArg("ADD", addr);
            removeIdentifier(bIndex.name);
        }
        else if(bIndex.type == "IDE") {
            setRegister(to_string(b.mem - b.begin));
            pushCommandOneArg("ADD", bIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("ADD", a.mem);
        }
    } else if(a.type == "ARR" && b.type == "IDE") { 
        if(aIndex.type == "NUM") {
        		long long int addr = a.mem + (stoi(aIndex.name) - a.begin);
            memToRegister(addr);
            pushCommandOneArg("ADD", b.mem);
            removeIdentifier(aIndex.name);
        }
        else if(aIndex.type == "IDE") {
            setRegister(to_string(a.mem - a.begin));
            pushCommandOneArg("ADD", aIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("ADD", b.mem);
        }
    } else if(a.type == "ARR" && b.type == "ARR") { 
        if(aIndex.type == "NUM" && bIndex.type == "NUM") {
            long long int addrA = a.mem + (stoi(aIndex.name) - a.begin);
            long long int addrB = b.mem + (stoi(bIndex.name) - b.begin);
            memToRegister(addrA);
            if(a.name == b.name && addrA == addrB)
                pushCommand("SHIFT 2");
            else 
                pushCommandOneArg("ADD", addrB);
            removeIdentifier(aIndex.name);
            removeIdentifier(bIndex.name);
        }
        else if(aIndex.type == "NUM" && bIndex.type == "IDE") {
            long long int addrA = a.mem + (stoi(aIndex.name) - a.begin);  
            setRegister(to_string((b.mem - b.begin)));
            pushCommandOneArg("ADD", bIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("ADD", addrA);
            removeIdentifier(aIndex.name);
        }
        else if(aIndex.type == "IDE" && bIndex.type == "NUM") {
            long long int addrB = b.mem + (stoi(bIndex.name) - b.begin);  
            setRegister(to_string((a.mem - a.begin)));
            pushCommandOneArg("ADD", aIndex.mem);
            pushCommandOneArg("LOADI", 0);
            pushCommandOneArg("ADD", addrB);
            removeIdentifier(bIndex.name);
        }
        else if(aIndex.type == "IDE" && bIndex.type == "IDE") {
            if(a.name == b.name && aIndex.name == bIndex.name) {
                setRegister(to_string((a.mem - a.begin)));
            	 pushCommandOneArg("ADD", aIndex.mem);
            	 pushCommandOneArg("LOADI", 0);
                pushCommand("SHIFT 2");
            }
            else {
            	 setRegister(to_string((a.mem - a.begin)));
            	 pushCommandOneArg("ADD", aIndex.mem);
            	 pushCommandOneArg("LOADI", 0);
            	 registerToMem(3); 
            	 setRegister(to_string((b.mem - b.begin)));
            	 pushCommandOneArg("ADD", bIndex.mem);
            	 pushCommandOneArg("LOADI", 0);
            	 pushCommandOneArg("ADD", 3);
            }
        }
   } 
} 		
}

void read() {
	pushCommand("GET");
	assign();
}

void assign() {
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
                setRegister(to_string(assignTarget.mem - assignTarget.begin));
            	 pushCommandOneArg("ADD", index.mem);
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
                long long int tabElMem = value.mem + (stoi(index.name) - value.begin);
                
                memToRegister(tabElMem);
                removeIdentifier(index.name);
            }
            else {
                setRegister(to_string(value.mem - value.begin));
            	 pushCommandOneArg("ADD", index.mem);
            	 pushCommandOneArg("LOADI", 0);            
            }
        }
        
   expressionArguments[0] = "null";
  	argumentsTabIndex[0] = "null"; 
   if (writeFlag) {
   	 pushCommand("PUT");
   	 assignFlag = 1;
       writeFlag = 0;
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
    if (expressionArguments[0] == "null"){
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
            //setRegister(to_string(s.mem+1));
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
                if (expressionArguments[0] == "null"){
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
         if (expressionArguments[0] == "null"){
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
                if (expressionArguments[0] == "null"){
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
        memCounter++;
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

void printCode(string outFileName) {
   ofstream out_code(outFileName);
	long long int i;
	for(i = 0; i < codeStack.size(); i++)
        out_code << codeStack.at(i) << endl;
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
    string file = argc[2];
    printCode(file);
    cout << "\n#Compiled without errors \n" << endl;
    }
	return 0;
}



