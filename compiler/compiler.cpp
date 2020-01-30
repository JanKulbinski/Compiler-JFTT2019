#include "compiler.hpp"

vector<string> codeVector;
vector<Jump> jumpVector;
vector<string> forVector;
map<string, Identifier> identifierMap;

int assignFlag;
int writeFlag;
long long int memCounter;
long long int depth;
Identifier assignTarget;
string tabAssignTargetIndex;
string expressionArguments[2];
string argumentsTabIndex[2];

void setUp() {
  writeFlag = 0;
  assignFlag = 1;
  memCounter = 15;
  depth = 0;

  //create 0
  pushCommandOneArg("SUB", 0);
  registerToMem(1);

  //create 1
  pushCommand("INC");
  registerToMem(2);

  //create -1
  pushCommand("DEC");
  pushCommand("DEC");
  registerToMem(9);
  Identifier assignTarget;
  tabAssignTargetIndex = "null";
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
  argumentsTabIndex[0] = "null";
  argumentsTabIndex[1] = "null";
}

void zeroRegister() {
  memToRegister(1);
}

void registerToMem(long long int mem) {
  pushCommandOneArg("STORE", mem);
}

void memToRegister(long long int mem) {
  pushCommandOneArg("LOAD", mem);
}

void pushCommand(string str) {
  codeVector.push_back(str);
}

void pushCommandOneArg(string str, long long int num) {
  string temp = str + " " + to_string(num);
  codeVector.push_back(temp);
}

void printCode(string outFileName) {
  ofstream out_code(outFileName);
  long long int i;
  for (i = 0; i < codeVector.size(); i++)
    out_code << codeVector.at(i) << endl;
}

void read(int yylineno) {
  pushCommand("GET");
  assign(yylineno);
}

void setToRegister(Identifier a, Identifier aI) {
  if (a.type == "NUM") {
    setRegister(a.name);
  } else if (a.type == "IDE") {
    memToRegister(a.mem);
  } else if (a.type == "ARR" && aI.type == "NUM") {
    long long int addr = a.mem + (stoll(aI.name) - a.begin);
    memToRegister(addr);
  } else if (a.type == "ARR" && aI.type == "IDE") {
    setRegister(to_string(a.mem - a.begin));
    pushCommandOneArg("ADD", aI.mem);
    pushCommandOneArg("LOADI", 0);
  }
}

void setRegister(string number) {
  long long int n = stoll(number);
  string bin = decToBin(n);
  long long int limit = bin.size();
  zeroRegister();

  if (limit <= 4) {
    for (int i = 0; i < abs(n); i++) {
      if (n < 0)
        pushCommand("DEC");
      else
        pushCommand("INC");
    }
  } else {
    for (long long int i = 0; i < limit; ++i) {
      if (bin[i] == '1') {
        if (n > 0)
          pushCommand("INC");
        else
          pushCommand("DEC");
      }
      if (i < (limit - 1)) {
        pushCommandOneArg("SHIFT", 2);
      }
    }
  }
}

long long int setToTempMem(Identifier a, Identifier aI, long long int tempMem) {
  
  long long int mem = 0;
  if (a.type == "NUM") {
    setRegister(a.name);
    registerToMem(tempMem);
    removeIdentifier(a.name);
    
  } else if (a.type == "IDE") {
    memToRegister(a.mem);
    registerToMem(tempMem);
    
  } else if (a.type == "ARR" && aI.type == "NUM") {
    long long int addr = a.mem + (stoll(aI.name) - a.begin);
    memToRegister(addr);
    registerToMem(tempMem);
    removeIdentifier(aI.name);
    
  } else if (a.type == "ARR" && aI.type == "IDE") {
    setRegister(to_string(a.mem - a.begin));
    pushCommandOneArg("ADD", aI.mem);
    pushCommandOneArg("LOADI", 0);
    registerToMem(tempMem);
  }
  return mem;
}

// IF STATMENT
void elseBlock() {

  Jump j;
  createJump( & j, codeVector.size(), depth);
  jumpVector.push_back(j);
  pushCommand("JUMP");

  long long int jumpCount = jumpVector.size() - 2;
  Jump jump = jumpVector.at(jumpCount);
  addInt(jump.placeInStack, codeVector.size());
  jumpCount--;
  if (jumpCount >= 0 && jumpVector.at(jumpCount).depth == depth) {
    addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
  }
  assignFlag = 1;
}

void elseEndIfBlock() {

  addInt(jumpVector.at(jumpVector.size() - 1).placeInStack, codeVector.size());
  jumpVector.pop_back();
  jumpVector.pop_back();
  if (jumpVector.size() >= 1 && jumpVector.at(jumpVector.size() - 1).depth == depth) {
    jumpVector.pop_back();
  }
  depth--;
  assignFlag = 1;
}

void endIfBlock() {

  long long int jumpCount = jumpVector.size() - 1;
  addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
  jumpCount--;
  if (jumpCount >= 0 && jumpVector.at(jumpCount).depth == depth) {
    addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
    jumpVector.pop_back();
  }
  jumpVector.pop_back();
  depth--;
  assignFlag = 1;
}

//ASSIGNING
void assign(int yylineno) {
  if (assignTarget.type == "ARR") {

    Identifier index = identifierMap.at(tabAssignTargetIndex);
    if (index.type == "NUM") {
      if (stoll(index.name) < assignTarget.begin || stoll(index.name) >= assignTarget.begin + assignTarget.tableSize) {
       cout << assignTarget.tableSize << endl;
       cout << assignTarget.begin + assignTarget.tableSize << endl;
        cout << "Line: " << yylineno << ". Error: Element outside of array ";
        cout << assignTarget.name << "[" << stoll(index.name) << "]" << endl;
      }
      long long int tabElMem = assignTarget.mem + (stoll(index.name) - assignTarget.begin);
      registerToMem(tabElMem);
      removeIdentifier(index.name);
    } else {
      registerToMem(3);
      setRegister(to_string(assignTarget.mem - assignTarget.begin));
      pushCommandOneArg("ADD", index.mem);
      registerToMem(4);
      memToRegister(3);
      pushCommandOneArg("STOREI", 4);
    }
  } else if (assignTarget.local == 0) {
    registerToMem(assignTarget.mem);
  } else {
    cout << "Line: " << yylineno << ". Error: Attempt to modify loop's iterator " << assignTarget.name << endl;
    exit(1);
  }

  identifierMap.at(assignTarget.name).initialized = 1;
  assignFlag = 1;
}

void soloValue() {
  Identifier value = identifierMap.at(expressionArguments[0]);

  if (value.type == "NUM") {
    setRegister(value.name);

    removeIdentifier(value.name);
  } else if (value.type == "IDE") {
    memToRegister(value.mem);
  } else {
    Identifier index = identifierMap.at(argumentsTabIndex[0]);
    if (index.type == "NUM") {
      long long int tabElMem = value.mem + (stoll(index.name) - value.begin);

      memToRegister(tabElMem);
      removeIdentifier(index.name);
    } else {
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
    cout << "Line: " << yylineno << ". Error: can't assign to" << variable << endl;
    exit(1);
  }
  Identifier s;
  createIdentifier( & s, variable, 0, 0, "NUM", 0);
  insertIdentifier(variable, s);
  if (expressionArguments[0] == "null") {
    expressionArguments[0] = variable;
  } else {
    expressionArguments[1] = variable;
  }
}

//DECLARATION
void declareIdent(string variable, int yylineno) {
  if (identifierMap.find(variable) != identifierMap.end()) {
    cout << "Line: " << yylineno << ". Error: Variable " << variable <<" has been already declared"<<endl;
    exit(1);
  } else {
    Identifier s;
    createIdentifier( & s, variable, 0, 0, "IDE", 0);
    insertIdentifier(variable, s);
  }
}

void declareArr(string variable, string begin, string end, int yylineno) {

  if (identifierMap.find(variable) != identifierMap.end()) {
    cout << "Line: " << yylineno << ". Error: Variable " << variable <<" has been already declared"<<endl;
    exit(1);
  } else if (stoll(begin) > stoll(end)) {
    cout << "Line: " << yylineno << ". Error: Array's begin index is bigger than its end index: " << variable << endl;
    exit(1);
  } else {
    long long int size = stoll(end) - stoll(begin) + 1;
    Identifier s;
    createIdentifier( & s, variable, 0, size, "ARR", stoll(begin));
    insertIdentifier(variable, s);
    memCounter += size - 1;
  }
}

void insertIdentifier(string key, Identifier i) {
  if (identifierMap.count(key) == 0) {
    identifierMap.insert(make_pair(key, i));
    identifierMap.at(key).counter = 0;
    memCounter++;
  } else {
    identifierMap.at(key).counter = identifierMap.at(key).counter + 1;
  }
}

void createIdentifier(Identifier * s, string name, long long int isLocal,
  long long int isArray, string type, long long int begin) {
  s->name = name;
  s->mem = memCounter;
  s->type = type;
  s->initialized = 0;

  if (isLocal) {
    s->local = 1;
  } else {
    s->local = 0;
  }

  if (isArray) {
    s->tableSize = isArray;
  } else {
    s->tableSize = 0;
  }

  s->begin = begin;
}

void removeIdentifier(string key) {
  if (identifierMap.count(key) > 0) {
    if (identifierMap.at(key).counter > 0) {
      identifierMap.at(key).counter = identifierMap.at(key).counter - 1;
    } else {
      identifierMap.erase(key);
      memCounter--;
    }
  }
}

void createJump(Jump * j, long long int stack, long long int depth) {
  j->placeInStack = stack;
  j-> depth = depth;
}

void addInt(long long int command, long long int val) {
  codeVector.at(command) = codeVector.at(command) + " " + to_string(val);
}

void ident(string variable, int yylineno) {

  if (identifierMap.find(variable) == identifierMap.end()) {
    cout << "Line: " << yylineno << ". Error: Undeclared variable " << variable << endl;
    exit(1);
  }

  if (identifierMap.at(variable).tableSize == 0) {
    if (!assignFlag) {
      if (identifierMap.at(variable).initialized == 0) {
        cout << "Line: " << yylineno << ". Error: Attempt to use uninitialized value " << variable << endl;
        exit(1);
      }
      if (expressionArguments[0] == "null") {
        expressionArguments[0] = variable;
      } else {
        expressionArguments[1] = variable;
      }

    } else {
      assignTarget = identifierMap.at(variable);
    }
  } else {
    cout << "Line: " << yylineno << ". Error: " << variable << " is an array" << endl;
    exit(1);
  }
}

void identIdent(string tab, string i, int yylineno) {

  if (identifierMap.find(tab) == identifierMap.end()) {
    cout << "Line: " << yylineno << ". Error: Undecleared variable " << tab << endl;
    exit(1);
  }
  
  if (identifierMap.find(i) == identifierMap.end()) {
    cout << "Line: " << yylineno << ". Error: Undecleared variable " << i << endl;
    exit(1);
  }

  if (identifierMap.at(tab).tableSize == 0) {
    cout << "Line: " << yylineno << ". Error: Variable " << tab << " is not an array" << endl;
    exit(1);
  }

  if (identifierMap.at(i).initialized == 0) {
  	 cout << "Line: " << yylineno << ". Error: Attempt to use uninitialized value " << i << endl;
    exit(1);
  }

  if (!assignFlag) {
    if (expressionArguments[0] == "null") {
      expressionArguments[0] = tab;
      argumentsTabIndex[0] = i;
    } else {
      expressionArguments[1] = tab;
      argumentsTabIndex[1] = i;
    }
  } else {
    assignTarget = identifierMap.at(tab);
    tabAssignTargetIndex = i;
  }
}

void identNum(string tab, string num, int yylineno) {
  if (identifierMap.find(tab) == identifierMap.end()) {
  	 cout << "Line: " << yylineno << ". Error: Undecleared variable " << tab << endl;
    exit(1);
  }

  if (identifierMap.at(tab).tableSize == 0) {
    cout << "Line: " << yylineno << ". Error: Variable " << tab <<" is not an array." << endl;
    exit(1);
  } else {
    Identifier s;
    createIdentifier( & s, num, 0, 0, "NUM", 0);
    insertIdentifier(num, s);

    if (!assignFlag) {
      if (expressionArguments[0] == "null") {
        expressionArguments[0] = tab;
        argumentsTabIndex[0] = num;
      } else {
        expressionArguments[1] = tab;
        argumentsTabIndex[1] = num;
      }

    } else {
      assignTarget = identifierMap.at(tab);
      tabAssignTargetIndex = num;
    }
  }
}

string decToBin(long long int n) {
  string r;
  while (n != 0) {
    r = (n % 2 == 0 ? "0" : "1") + r;
    n /= 2;
  }
  return r;
}

long long int isPowerOf2(string value) {
  string bin = decToBin(stoll(value));
  long long int length = bin.size();
  if (bin[0] == '1' && bin.substr(1).find("1") == string::npos)
    return length;
  else
    return 0;
}

