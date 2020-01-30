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
   string type;
   int initialized;
   int local;
   long long int begin;
	long long int mem;
  	long long int tableSize;
  	long long int counter;
} Identifier;

typedef struct {
    long long int placeInStack;
    long long int depth;
} Jump;

extern vector<string> codeVector;
extern vector<Jump> jumpVector;
extern vector<string> forVector;
extern map<string, Identifier> identifierMap;

extern int assignFlag;
extern int writeFlag;
extern long long int memCounter;
extern long long int depth;
extern Identifier assignTarget;
extern string tabAssignTargetIndex;
extern string expressionArguments[2];
extern string argumentsTabIndex[2];

//BISON
void ident(string variable, int yylineo);
void identIdent(string tab, string i, int yylineo);
void identNum(string tab, string num, int yylineo);
void declareIdent(string variable, int yylineno);
void declareArr(string variable, string begin, string end, int yylineno);
void number(string variable, int yylineno);
void assign(int yylineno);
void read(int yylineno);
void soloValue();
void add();
void subtract();
void multiply();
void subtractIdentifires(string a, string b);
void divide();
void modulo();

void elseBlock();
void elseEndIfBlock();
void endIfBlock();
void ifEqual();
void ifNotEqual();
void ifLess(); 
void ifGreater(); 
void ifLessEqual(); 
void ifGreaterEqual();

void whileDo();
void whileBegin();
void doWhile();
void forBegin(string variable, int yylineno);
void forTo();
void forDownTo();
void forEnd();

//HELPERS
string decToBin(long long int n);
void setUp();
void createIdentifier(Identifier *s, string name, long long int isLocal, long long int isArray, string type, long long int begin);
void insertIdentifier(string key, Identifier i);
void removeIdentifier(string key);
void printCode(string outFileName);
void createJump(Jump *j, long long int stack, long long int depth);
long long int setToTempMem(Identifier a, Identifier aI, long long int tempMem);
long long int isPowerOf2(string value);

//VIRTUAL MACHINE
void registerToMem(long long int mem);
void memToRegister(long long int mem);
void setRegister(string number);
void zeroRegister();
void pushCommand(string str);
void pushCommandOneArg(string str, long long int num);
void setToRegister(Identifier a, Identifier aI);
void addInt(long long int command, long long int val);
