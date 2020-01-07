#include "compiler.hpp"

void forBegin(string variable, int yylineno) {

  if (identifierStack.find(variable) != identifierStack.end()) {
    cout << "Line: " << yylineno << ". Error: Variable " << variable <<" has been already declared"<<endl;
    exit(1);
  } else {
    Identifier s;
    createIdentifier( &s, variable, 1, 0, "IDE", 0);
    s.initialized = 1;
    insertIdentifier(variable, s);
    forStack.push_back(variable);
  }

  assignFlag = 0;
  assignTarget = identifierStack.at(variable);
  depth++;
}

void forTo() {

  Identifier a = identifierStack.at(expressionArguments[0]);
  if (a.type == "NUM") {
    setRegister(a.name);
  } else if (a.type == "IDE") {
    memToRegister(a.mem);
  } else {
    Identifier index = identifierStack.at(argumentsTabIndex[0]);
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

  Identifier b = identifierStack.at(expressionArguments[1]);
  if (b.type == "NUM") {
    setRegister(b.name);
  } else if (b.type == "IDE") {
    memToRegister(b.mem);
  } else {
    Identifier index = identifierStack.at(argumentsTabIndex[1]);
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
  createIdentifier( & s, "L" + depth, 0, 0, "IDE", 0);
  insertIdentifier("L" + depth, s);
  s.initialized = 1;
  registerToMem(s.mem);

  Jump j;
  createJump( & j, codeStack.size(), depth);
  jumpStack.push_back(j);
  memToRegister(assignTarget.mem);
  pushCommand("INC");
  registerToMem(assignTarget.mem);
  subtractIdentifires(assignTarget.name, "L" + depth);

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JPOS");
  assignFlag = 1;
  
  //  argumentsTabIndex[0] = "null";
 // argumentsTabIndex[1] = "null";
 // expressionArguments[0] = "null";
//  expressionArguments[1] = "null";
  
}

void forDownTo() {

  Identifier a = identifierStack.at(expressionArguments[0]);
  if (a.type == "NUM") {
    setRegister(a.name);
  } else if (a.type == "IDE") {
    memToRegister(a.mem);
  } else {
    Identifier index = identifierStack.at(argumentsTabIndex[0]);
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

  Identifier b = identifierStack.at(expressionArguments[1]);
  if (b.type == "NUM") {
    setRegister(b.name);
  } else if (b.type == "IDE") {
    memToRegister(b.mem);
  } else {
    Identifier index = identifierStack.at(argumentsTabIndex[1]);
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
  createIdentifier( & s, "L" + depth, 0, 0, "IDE", 0);
  insertIdentifier("L" + depth, s);
  s.initialized = 1;
  registerToMem(s.mem);

  Jump j;
  createJump( & j, codeStack.size(), depth);
  jumpStack.push_back(j);
  memToRegister(assignTarget.mem);
  pushCommand("DEC");
  registerToMem(assignTarget.mem);
  subtractIdentifires(assignTarget.name, "L" + depth);

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JNEG");
  assignFlag = 1;
}

void forEnd() {

  long long int stack;
  long long int jumpCount = jumpStack.size() - 1;

  stack = jumpStack.at(jumpCount - 1).placeInStack;
  pushCommandOneArg("JUMP", stack);
  addInt(jumpStack.at(jumpCount).placeInStack, codeStack.size());
  jumpStack.pop_back();
  jumpStack.pop_back();

  removeIdentifier("L" + depth);
  removeIdentifier(forStack.back());
  forStack.pop_back();
  depth--;
}

void whileBegin() {
  depth++;
  Jump j;
  createJump( & j, codeStack.size(), depth);
  jumpStack.push_back(j);
}

void doWhile() {

  long long int stack;
  long long int jumpCount = jumpStack.size() - 1;

  if (jumpCount >= 3 && jumpStack.at(jumpCount - 3).depth == depth - 1) {
    stack = jumpStack.at(jumpCount - 3).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpStack.at(jumpCount).placeInStack, codeStack.size());
    addInt(jumpStack.at(jumpCount - 1).placeInStack, codeStack.size());
    jumpStack.pop_back();

  } else {
    stack = jumpStack.at(jumpCount - 2).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpStack.at(jumpCount).placeInStack, codeStack.size());
  }
  jumpStack.pop_back();
  jumpStack.pop_back();
  jumpStack.pop_back();
  depth--;
  depth--;
  assignFlag = 1;
}

void whileDo() {

  long long int stack;
  long long int jumpCount = jumpStack.size() - 1;

  if (jumpCount >= 2 && jumpStack.at(jumpCount - 2).depth == depth) {
    stack = jumpStack.at(jumpCount - 2).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpStack.at(jumpCount).placeInStack, codeStack.size());
    addInt(jumpStack.at(jumpCount - 1).placeInStack, codeStack.size());
    jumpStack.pop_back();
  } else {
    stack = jumpStack.at(jumpCount - 1).placeInStack;
    pushCommandOneArg("JUMP", stack);

    addInt(jumpStack.at(jumpCount).placeInStack, codeStack.size());
  }
  jumpStack.pop_back();
  jumpStack.pop_back();
  depth--;
  assignFlag = 1;
}

