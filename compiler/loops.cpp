#include "compiler.hpp"

void forBegin(string variable, int yylineno) {

  if (identifierMap.find(variable) != identifierMap.end()) {
    cout << "Line: " << yylineno << ". Error: Variable " << variable <<" has been already declared"<<endl;
    exit(1);
  } else {
    Identifier s;
    createIdentifier( &s, variable, 1, 0, "IDE", 0);
    s.initialized = 1;
    insertIdentifier(variable, s);
    forVector.push_back(variable);
  }

  assignFlag = 0;
  assignTarget = identifierMap.at(variable);
  depth++;
}

void forTo() {

  Identifier a = identifierMap.at(expressionArguments[0]);
  if (a.type == "NUM") {
    setRegister(a.name);
  } else if (a.type == "IDE") {
    memToRegister(a.mem);
  } else {
    Identifier index = identifierMap.at(argumentsTabIndex[0]);
    if (index.type == "NUM") {
      long long int tabElMem = a.mem + (stoll(index.name) - a.begin);
      memToRegister(tabElMem);
    } else {
      setRegister(to_string(a.mem - a.begin));
      pushCommandOneArg("ADD", index.mem);
      pushCommandOneArg("LOADI", 0);
    }
  }
  pushCommand("DEC");
  registerToMem(assignTarget.mem);
  assignTarget.initialized = 1;

  Identifier b = identifierMap.at(expressionArguments[1]);
  if (b.type == "NUM") {
    setRegister(b.name);
  } else if (b.type == "IDE") {
    memToRegister(b.mem);
  } else {
    Identifier index = identifierMap.at(argumentsTabIndex[1]);
    if (index.type == "NUM") {
      long long int tabElMem = b.mem + (stoll(index.name) - b.begin);
      memToRegister(tabElMem);
    } else {
      setRegister(to_string(b.mem - b.begin));
      pushCommandOneArg("ADD", index.mem);
      pushCommandOneArg("LOADI", 0);
    }
  }

  Identifier s;
  string limitVariable = "L" + to_string(depth);
  createIdentifier( &s, limitVariable, 0, 0, "IDE", 0);
  insertIdentifier(limitVariable, s);
  
  s.initialized = 1;
  registerToMem(s.mem);

  Jump j;
  createJump( &j, codeVector.size(), depth);
  jumpVector.push_back(j);
  memToRegister(assignTarget.mem);
  pushCommand("INC");
  registerToMem(assignTarget.mem);
  subtractIdentifires(assignTarget.name,limitVariable);

  Jump jump;
  createJump( &jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JPOS");
  assignFlag = 1;
  
}

void forDownTo() {

  Identifier a = identifierMap.at(expressionArguments[0]);
  if (a.type == "NUM") {
    setRegister(a.name);
  } else if (a.type == "IDE") {
    memToRegister(a.mem);
  } else {
    Identifier index = identifierMap.at(argumentsTabIndex[0]);
    if (index.type == "NUM") {
      long long int tabElMem = a.mem + (stoll(index.name) - a.begin);
      memToRegister(tabElMem);
    } else {
      setRegister(to_string(a.mem - a.begin));
      pushCommandOneArg("ADD", index.mem);
      pushCommandOneArg("LOADI", 0);
    }
  }
  pushCommand("INC");
  registerToMem(assignTarget.mem);
  assignTarget.initialized = 1;

  Identifier b = identifierMap.at(expressionArguments[1]);
  if (b.type == "NUM") {
    setRegister(b.name);
  } else if (b.type == "IDE") {
    memToRegister(b.mem);
  } else {
    Identifier index = identifierMap.at(argumentsTabIndex[1]);
    if (index.type == "NUM") {
      long long int tabElMem = b.mem + (stoll(index.name) - b.begin);
      memToRegister(tabElMem);
    } else {
      setRegister(to_string(b.mem - b.begin));
      pushCommandOneArg("ADD", index.mem);
      pushCommandOneArg("LOADI", 0);
    }
  }

  Identifier s;
  string limitVariable = "L" + to_string(depth);
  createIdentifier( & s, limitVariable, 0, 0, "IDE", 0);
  insertIdentifier(limitVariable, s);
  s.initialized = 1;
  registerToMem(s.mem);

  Jump j;
  createJump( & j, codeVector.size(), depth);
  jumpVector.push_back(j);
  memToRegister(assignTarget.mem);
  pushCommand("DEC");
  registerToMem(assignTarget.mem);

  subtractIdentifires(assignTarget.name, limitVariable);
  
  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JNEG");
  assignFlag = 1;
}

void forEnd() {

  long long int stack;
  long long int jumpCount = jumpVector.size() - 1;

  stack = jumpVector.at(jumpCount - 1).placeInStack;
  pushCommandOneArg("JUMP", stack);
  addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
  jumpVector.pop_back();
  jumpVector.pop_back();
  removeIdentifier("L" + to_string(depth));
  removeIdentifier(forVector.back());
  forVector.pop_back();
  depth--;
}

void whileBegin() {
  depth++;
  Jump j;
  createJump( & j, codeVector.size(), depth);
  jumpVector.push_back(j);
}

void doWhile() {

  long long int stack;
  long long int jumpCount = jumpVector.size() - 1;

  if (jumpCount >= 3 && jumpVector.at(jumpCount - 3).depth == depth - 1) {
    stack = jumpVector.at(jumpCount - 3).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
    addInt(jumpVector.at(jumpCount - 1).placeInStack, codeVector.size());
    jumpVector.pop_back();

  } else {
    stack = jumpVector.at(jumpCount - 2).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
  }
  jumpVector.pop_back();
  jumpVector.pop_back();
  jumpVector.pop_back();
  depth--;
  depth--;
  assignFlag = 1;
}

void whileDo() {

  long long int stack;
  long long int jumpCount = jumpVector.size() - 1;

  if (jumpCount >= 2 && jumpVector.at(jumpCount - 2).depth == depth) {
    stack = jumpVector.at(jumpCount - 2).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
    addInt(jumpVector.at(jumpCount - 1).placeInStack, codeVector.size());
    jumpVector.pop_back();
  } else {
    stack = jumpVector.at(jumpCount - 1).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpVector.at(jumpCount).placeInStack, codeVector.size());
  }
  jumpVector.pop_back();
  jumpVector.pop_back();
  depth--;
  assignFlag = 1;
}

