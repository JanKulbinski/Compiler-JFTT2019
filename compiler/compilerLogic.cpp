#include "compilerLogic.hpp"

//functions impl...



map<string, Identifier> identifierStack;

int assignFlag;
Identifier assignTarget;
//string tabAssignTargetIndex = "-1";
string expressionArguments[2] = {"-1", "-1"};
//string argumentsTabIndex[2] = {"-1", "-1"};

void setUp() {
		assiginFlag = 1;
}

void ident(char* variable, int yylineo) {
{
        if(identifierStack.find(variable) == identifierStack.end()) {
            cout << "Line: " << yylineno << " Error. Undeclared variable." << variable << endl;
            exit(1);
        }
        
        if(identifierStack.at(variable).tableSize == 0) {
            if(!assignFlag){
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
