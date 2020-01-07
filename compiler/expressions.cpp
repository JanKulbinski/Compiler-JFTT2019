#include "compiler.hpp"

void modulo() {

  Identifier a = identifierStack.at(expressionArguments[0]);
  Identifier b = identifierStack.at(expressionArguments[1]);

  Identifier aI, bI;
  if (identifierStack.count(argumentsTabIndex[0]) > 0)
    aI = identifierStack.at(argumentsTabIndex[0]);
  if (identifierStack.count(argumentsTabIndex[1]) > 0)
    bI = identifierStack.at(argumentsTabIndex[1]);

  if (a.type == "NUM" && stoll(a.name) == 0) {
    zeroRegister();
  } else if (b.type == "NUM" && stoll(b.name) == 0) {
    zeroRegister();
  } else if (a.type == "NUM" && b.type == "NUM") {
    long long int val = stoll(a.name) % stoll(b.name);
    if (stoll(b.name) > 0)
      setRegister(to_string(val));
    else
      setRegister(to_string(-val));
    removeIdentifier(a.name);
    removeIdentifier(b.name);

  } else {
    zeroRegister();
    registerToMem(7);
    registerToMem(4);
    pushCommand("DEC");
    registerToMem(8);
    setToTempMem(a, aI, 5);
    setToTempMem(b, bI, 6);

    //if (b < 0) b = -b         	
    pushCommandOneArg("JPOS", codeStack.size() + 8);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(6);
    pushCommandOneArg("SUB", 3);
    registerToMem(6);
    zeroRegister();
    registerToMem(8);

    //if (a < 0) a = -a
    memToRegister(5);
    pushCommandOneArg("JPOS", codeStack.size() + 6);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(5);
    pushCommandOneArg("SUB", 3);
    registerToMem(5);

    // R = a
    registerToMem(10);
    // if(|a| < |b|) return a
    pushCommandOneArg("SUB", 6);
    pushCommandOneArg("JPOS", codeStack.size() + 5);
    pushCommandOneArg("JZERO", codeStack.size() + 4);
    zeroRegister();
    registerToMem(12);
    pushCommandOneArg("JUMP", codeStack.size() + 49);

    //R = a
    memToRegister(5);
    //registerToMem(10);		
    registerToMem(11);

    //compute n = #bits of a
    memToRegister(11);
    pushCommandOneArg("SHIFT", 9);
    pushCommandOneArg("JZERO", codeStack.size() + 6);
    registerToMem(11);
    memToRegister(4);
    pushCommand("INC");
    registerToMem(4);
    pushCommandOneArg("JUMP", codeStack.size() - 7);

    //b = b << n
    memToRegister(4);
    pushCommand("INC");
    registerToMem(4);
    memToRegister(6);
    pushCommandOneArg("SHIFT", 4);
    registerToMem(11);
    memToRegister(4);
    registerToMem(12);
    //(12) <- -n
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(12);
    pushCommandOneArg("SUB", 3);
    registerToMem(12);

    memToRegister(4);
    pushCommand("DEC");
    // for n-1 .. 0
    int stackJ1 = codeStack.size();
    pushCommandOneArg("JNEG", codeStack.size() + 23);
    registerToMem(4);

    // R = R*2 - D
    memToRegister(10);
    pushCommandOneArg("SHIFT", 2);
    pushCommandOneArg("SUB", 11);
    registerToMem(10);

    // if R >= 0
    pushCommandOneArg("JNEG", codeStack.size() + 8);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    pushCommand("INC");
    registerToMem(7);
    memToRegister(4);
    pushCommand("DEC");
    pushCommandOneArg("JUMP", stackJ1);

    memToRegister(10);
    pushCommandOneArg("ADD", 11);
    registerToMem(10);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(7);
    memToRegister(4);
    pushCommand("DEC");
    pushCommandOneArg("JUMP", stackJ1);

    //if(a.old * b.old < 0) c = -c 
    memToRegister(8);
    pushCommandOneArg("JNEG", codeStack.size() + 7);
    memToRegister(10);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(10);
    pushCommandOneArg("SUB", 3);
    pushCommandOneArg("JUMP", codeStack.size() + 2);

    //R = R / 2^(#bits of a)
    memToRegister(10);
    pushCommandOneArg("SHIFT", 12);
  }

  argumentsTabIndex[0] = "null";
  argumentsTabIndex[1] = "null";
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
}

void multiplication() {

  Identifier a = identifierStack.at(expressionArguments[0]);
  Identifier b = identifierStack.at(expressionArguments[1]);

  Identifier aI, bI;
  if (identifierStack.count(argumentsTabIndex[0]) > 0)
    aI = identifierStack.at(argumentsTabIndex[0]);
  if (identifierStack.count(argumentsTabIndex[1]) > 0)
    bI = identifierStack.at(argumentsTabIndex[1]);

  if (a.type == "NUM" && b.type == "NUM") {
    long long int val = stoll(a.name) * stoll(b.name);
    setRegister(to_string(val));
    removeIdentifier(a.name);
    removeIdentifier(b.name);

  } else if (a.type == "NUM" && isPowerOf2(a.name)) {
    setRegister(to_string(isPowerOf2(a.name) - 1));
    registerToMem(3);
    setToRegister(b, bI);

    if (stoll(a.name) < 0) {
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("SHIFT", 2);
      registerToMem(4);
      setToRegister(b, bI);
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("SUB", 4);

    } else if (stoll(a.name) > 0) {
      pushCommandOneArg("SHIFT", 3);
    } else {
      zeroRegister();
    }
    removeIdentifier(a.name);

  } else if (b.type == "NUM" && isPowerOf2(b.name)) {
    setRegister(to_string(isPowerOf2(b.name) - 1));
    registerToMem(3);
    setToRegister(a, aI);

    if (stoll(b.name) < 0) {
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("SHIFT", 2);
      registerToMem(4);
      setToRegister(a, aI);
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("SUB", 4);

    } else if (stoll(b.name) > 0) {
      pushCommandOneArg("SHIFT", 3);
    } else {
      zeroRegister();
    }
    removeIdentifier(b.name);

  } else {

    /* Optimalization plans: change order a,b if a > b*/
    zeroRegister();
    registerToMem(7);
    pushCommand("DEC");
    registerToMem(8);
    setToTempMem(a, aI, 5);
    setToTempMem(b, bI, 6);

    //if (b < 0) b = -b         	
    pushCommandOneArg("JPOS", codeStack.size() + 8);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(6);
    pushCommandOneArg("SUB", 3);
    registerToMem(6);
    zeroRegister();
    registerToMem(8);

    //if (a < 0) a = -a
    memToRegister(5);
    pushCommandOneArg("JPOS", codeStack.size() + 10);
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
    pushCommandOneArg("JZERO", codeStack.size() + 16);
    pushCommandOneArg("SHIFT", 9);
    pushCommandOneArg("SHIFT", 2);
    pushCommandOneArg("SUB", 5);
    pushCommandOneArg("JNEG", codeStack.size() + 2);
    pushCommandOneArg("JUMP", codeStack.size() + 4);
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
    pushCommandOneArg("JNEG", codeStack.size() + 8);
    pushCommandOneArg("JPOS", codeStack.size() + 7);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(7);
    pushCommandOneArg("SUB", 3);
    pushCommandOneArg("JUMP", codeStack.size() + 2);
    memToRegister(7);
  }
  argumentsTabIndex[0] = "null";
  argumentsTabIndex[1] = "null";
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
}

void divide() {

  Identifier a = identifierStack.at(expressionArguments[0]);
  Identifier b = identifierStack.at(expressionArguments[1]);

  Identifier aI, bI;
  if (identifierStack.count(argumentsTabIndex[0]) > 0)
    aI = identifierStack.at(argumentsTabIndex[0]);
  if (identifierStack.count(argumentsTabIndex[1]) > 0)
    bI = identifierStack.at(argumentsTabIndex[1]);

  if (a.type == "NUM" && stoll(a.name) == 0) {
    zeroRegister();
  } else if (b.type == "NUM" && stoll(b.name) == 0) {
    zeroRegister();
  } else if (a.type == "NUM" && b.type == "NUM") {
    long long int val = stoll(a.name) / stoll(b.name);
    setRegister(to_string(val));
    removeIdentifier(a.name);
    removeIdentifier(b.name);

  } /*else if (b.type == "NUM" && isPowerOf2(b.name)) {
    setRegister(to_string(1 - isPowerOf2(b.name)));
    registerToMem(3);
    setToRegister(a, aI);
    if (stoll(b.name) < 0) {
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("JPOS",codeStack.size()+3);
      pushCommandOneArg("JZERO",codeStack.size()+2);
      pushCommand("INC");
      pushCommandOneArg("SHIFT", 2);
      registerToMem(4);
      setToRegister(a, aI);
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("JPOS",codeStack.size()+3);
      pushCommandOneArg("JZERO",codeStack.size()+2);
      pushCommand("INC");
      pushCommandOneArg("SUB", 4);

    } else if (stoll(b.name) > 0) {
      pushCommandOneArg("SHIFT", 3);
      pushCommandOneArg("JPOS",codeStack.size()+3);
    	pushCommandOneArg("JZERO",codeStack.size()+2);
    	pushCommand("INC");
    } else {
      zeroRegister();
    }

    removeIdentifier(b.name);

  }*/ else {
    zeroRegister();
    registerToMem(7);
    registerToMem(4);
    pushCommand("DEC");
    registerToMem(8);
    setToTempMem(a, aI, 5);
    setToTempMem(b, bI, 6);

    //if (b < 0) b = -b         	
    pushCommandOneArg("JPOS", codeStack.size() + 8);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(6);
    pushCommandOneArg("SUB", 3);
    registerToMem(6);
    zeroRegister();
    registerToMem(8);

    //if (a < 0) a = -a
    memToRegister(5);
    pushCommandOneArg("JPOS", codeStack.size() + 10);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(5);
    pushCommandOneArg("SUB", 3);
    registerToMem(5);
    memToRegister(8);
    pushCommand("INC");
    registerToMem(8);
    memToRegister(5);

    // if(|a| < |b|) return 0
    pushCommandOneArg("SUB", 6);
    pushCommandOneArg("JPOS", codeStack.size() + 4);
    pushCommandOneArg("JZERO", codeStack.size() + 3);
    zeroRegister();
    pushCommandOneArg("JUMP", codeStack.size() + 53);

    //R = a
    memToRegister(5);
    registerToMem(10);
    registerToMem(11);

    //compute n = #bits of a
    memToRegister(11);
    pushCommandOneArg("SHIFT", 9);
    pushCommandOneArg("JZERO", codeStack.size() + 6);
    registerToMem(11);
    memToRegister(4);
    pushCommand("INC");
    registerToMem(4);
    pushCommandOneArg("JUMP", codeStack.size() - 7);

    //b = b << n
    memToRegister(4);
    pushCommand("INC");
    registerToMem(4);
    memToRegister(6);
    pushCommandOneArg("SHIFT", 4);
    registerToMem(11);
    memToRegister(4);
    pushCommand("DEC");

    // for n-1 .. 0
    int stackJ1 = codeStack.size();
    pushCommandOneArg("JNEG", codeStack.size() + 23);
    registerToMem(4);

    // R = R*2 - D
    memToRegister(10);
    pushCommandOneArg("SHIFT", 2);
    pushCommandOneArg("SUB", 11);
    registerToMem(10);

    // if R >= 0
    pushCommandOneArg("JNEG", codeStack.size() + 8);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    pushCommand("INC");
    registerToMem(7);
    memToRegister(4);
    pushCommand("DEC");
    pushCommandOneArg("JUMP", stackJ1);

    memToRegister(10);
    pushCommandOneArg("ADD", 11);
    registerToMem(10);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(7);
    memToRegister(4);
    pushCommand("DEC");
    pushCommandOneArg("JUMP", stackJ1);

    //if(a.old * b.old < 0) c = -c 
    memToRegister(8);
    pushCommandOneArg("JNEG", codeStack.size() + 8);
    pushCommandOneArg("JPOS", codeStack.size() + 7);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(7);
    pushCommandOneArg("SUB", 3);
    pushCommandOneArg("JUMP", codeStack.size() + 2);
    memToRegister(7);
  }


  argumentsTabIndex[0] = "null";
  argumentsTabIndex[1] = "null";
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
}

void subtractIdentifires(string a, string b) {
  expressionArguments[0] = a;
  expressionArguments[1] = b;
  subtract();
}

void subtract() {

  Identifier a = identifierStack.at(expressionArguments[0]);
  Identifier b = identifierStack.at(expressionArguments[1]);
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
  if (a.type != "ARR" && b.type != "ARR") {
    if (a.type == "NUM" && b.type == "NUM") {
      long long int val = stoll(a.name) - stoll(b.name);
      setRegister(to_string(val));
      removeIdentifier(a.name);
      removeIdentifier(b.name);
    } else if (a.type == "NUM" && b.type == "IDE") {
      setRegister(a.name);
      pushCommandOneArg("SUB", b.mem);
      removeIdentifier(a.name);
    } else if (a.type == "IDE" && b.type == "NUM") {
      setRegister(b.name);
      registerToMem(3);
      memToRegister(a.mem);
      pushCommandOneArg("SUB", 3);
      removeIdentifier(b.name);
    } else if (a.type == "IDE" && b.type == "IDE") {
      if (a.name == b.name) {
        zeroRegister();
      } else {
        memToRegister(a.mem);
        pushCommandOneArg("SUB", b.mem);
      }
    }
  } else {
    Identifier aIndex, bIndex;
    if (identifierStack.count(argumentsTabIndex[0]) > 0)
      aIndex = identifierStack.at(argumentsTabIndex[0]);
    if (identifierStack.count(argumentsTabIndex[1]) > 0)
      bIndex = identifierStack.at(argumentsTabIndex[1]);
    argumentsTabIndex[0] = "null";
    argumentsTabIndex[1] = "null";

    if (a.type == "NUM" && b.type == "ARR") {
      if (bIndex.type == "NUM") {
        long long int addr = b.mem + (stoll(bIndex.name) - b.begin);
        setRegister(a.name);
        pushCommandOneArg("SUB", addr);
        removeIdentifier(a.name);
        removeIdentifier(bIndex.name);

      } else if (bIndex.type == "IDE") {
        setRegister(to_string(b.mem - b.begin));
        pushCommandOneArg("ADD", bIndex.mem);
        pushCommandOneArg("LOADI", 0);
        registerToMem(3);
        setRegister(a.name);
        pushCommandOneArg("SUB", 3);
        removeIdentifier(a.name);
      }
    } else if (a.type == "ARR" && b.type == "NUM") {
      if (aIndex.type == "NUM") {
        long long int addr = a.mem + (stoll(aIndex.name) - a.begin);
        setRegister(b.name);
        registerToMem(3);
        memToRegister(addr);
        pushCommandOneArg("SUB", 3);
        removeIdentifier(b.name);
        removeIdentifier(aIndex.name);

      } else if (aIndex.type == "IDE") {
        setRegister(b.name);
        registerToMem(3);
        setRegister(to_string((a.mem - a.begin)));
        pushCommandOneArg("ADD", aIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("SUB", 3);
        removeIdentifier(b.name);
      }
    } else if (a.type == "IDE" && b.type == "ARR") {
      if (bIndex.type == "NUM") {
        long long int addr = b.mem + (stoll(bIndex.name) - b.begin);
        memToRegister(a.mem);
        pushCommandOneArg("SUB", addr);
        removeIdentifier(bIndex.name);
      } else if (bIndex.type == "IDE") {
        setRegister(to_string(b.mem - b.begin));
        pushCommandOneArg("ADD", bIndex.mem);
        pushCommandOneArg("LOADI", 0);
        registerToMem(3);
        memToRegister(a.mem);
        pushCommandOneArg("SUB", 3);
      }
    } else if (a.type == "ARR" && b.type == "IDE") {
      if (aIndex.type == "NUM") {
        long long int addr = a.mem + (stoll(aIndex.name) - a.begin);
        memToRegister(addr);
        pushCommandOneArg("SUB", b.mem);
        removeIdentifier(aIndex.name);
      } else if (aIndex.type == "IDE") {
        setRegister(to_string(a.mem - a.begin));
        pushCommandOneArg("ADD", aIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("SUB", b.mem);
      }
    } else if (a.type == "ARR" && b.type == "ARR") {
      if (aIndex.type == "NUM" && bIndex.type == "NUM") {
        long long int addrA = a.mem + (stoll(aIndex.name) - a.begin);
        long long int addrB = b.mem + (stoll(bIndex.name) - b.begin);

        if (a.name == b.name && addrA == addrB)
          zeroRegister();
        else {
          memToRegister(addrA);
          pushCommandOneArg("SUB", addrB);
        }
        removeIdentifier(aIndex.name);
        removeIdentifier(bIndex.name);
      } else if (aIndex.type == "NUM" && bIndex.type == "IDE") {
        long long int addrA = a.mem + (stoll(aIndex.name) - a.begin);
        setRegister(to_string((b.mem - b.begin)));
        pushCommandOneArg("ADD", bIndex.mem);
        pushCommandOneArg("LOADI", 0);
        registerToMem(3);
        memToRegister(addrA);
        pushCommandOneArg("SUB", 3);
        removeIdentifier(aIndex.name);
      } else if (aIndex.type == "IDE" && bIndex.type == "NUM") {
        long long int addrB = b.mem + (stoll(bIndex.name) - b.begin);
        setRegister(to_string((a.mem - a.begin)));
        pushCommandOneArg("ADD", aIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("SUB", addrB);
        removeIdentifier(bIndex.name);
      } else if (aIndex.type == "IDE" && bIndex.type == "IDE") {
        if (a.name == b.name && aIndex.name == bIndex.name)
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

  if (a.type != "ARR" && b.type != "ARR") {
    if (a.type == "NUM" && b.type == "NUM") {
      long long int val = stoll(a.name) + stoll(b.name);
      setRegister(to_string(val));
      removeIdentifier(a.name);
      removeIdentifier(b.name);
    } else if (a.type == "NUM" && b.type == "IDE") {
      setRegister(a.name);
      pushCommandOneArg("ADD", b.mem);
      removeIdentifier(a.name);
    } else if (a.type == "IDE" && b.type == "NUM") {
      setRegister(b.name);
      pushCommandOneArg("ADD", a.mem);
      removeIdentifier(b.name);
    } else if (a.type == "IDE" && b.type == "IDE") {
      if (a.name == b.name) {
        memToRegister(a.mem);
        pushCommand("SHIFT 2");
      } else {
        memToRegister(a.mem);
        pushCommandOneArg("ADD", b.mem);
      }
    }
  } else {
    Identifier aIndex, bIndex;
    if (identifierStack.count(argumentsTabIndex[0]) > 0)
      aIndex = identifierStack.at(argumentsTabIndex[0]);
    if (identifierStack.count(argumentsTabIndex[1]) > 0)
      bIndex = identifierStack.at(argumentsTabIndex[1]);
    argumentsTabIndex[0] = "null";
    argumentsTabIndex[1] = "null";

    if (a.type == "NUM" && b.type == "ARR") {
      if (bIndex.type == "NUM") {
        long long int addr = b.mem + (stoll(bIndex.name) - b.begin);
        setRegister(a.name);
        pushCommandOneArg("ADD", addr);
        removeIdentifier(a.name);
        removeIdentifier(bIndex.name);

      } else if (bIndex.type == "IDE") {
        setRegister(to_string(b.mem - b.begin));
        pushCommandOneArg("ADD", bIndex.mem);
        pushCommandOneArg("LOADI", 0);
        registerToMem(3);
        setRegister(a.name);
        pushCommandOneArg("ADD", 3);
        removeIdentifier(a.name);
      }
    } else if (a.type == "ARR" && b.type == "NUM") {
      if (aIndex.type == "NUM") {
        long long int addr = a.mem + (stoll(aIndex.name) - a.begin);
        setRegister(b.name);
        pushCommandOneArg("ADD", addr);
        removeIdentifier(b.name);
        removeIdentifier(aIndex.name);

      } else if (aIndex.type == "IDE") {
        setRegister(to_string((a.mem - a.begin)));
        pushCommandOneArg("ADD", aIndex.mem);
        pushCommandOneArg("LOADI", 0);
        registerToMem(3);
        setRegister(b.name);
        pushCommandOneArg("ADD", 3);
        removeIdentifier(b.name);
      }
    } else if (a.type == "IDE" && b.type == "ARR") {
      if (bIndex.type == "NUM") {
        long long int addr = b.mem + (stoll(bIndex.name) - b.begin);
        memToRegister(a.mem);
        pushCommandOneArg("ADD", addr);
        removeIdentifier(bIndex.name);
      } else if (bIndex.type == "IDE") {
        setRegister(to_string(b.mem - b.begin));
        pushCommandOneArg("ADD", bIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("ADD", a.mem);
      }
    } else if (a.type == "ARR" && b.type == "IDE") {
      if (aIndex.type == "NUM") {
        long long int addr = a.mem + (stoll(aIndex.name) - a.begin);
        memToRegister(addr);
        pushCommandOneArg("ADD", b.mem);
        removeIdentifier(aIndex.name);
      } else if (aIndex.type == "IDE") {
        setRegister(to_string(a.mem - a.begin));
        pushCommandOneArg("ADD", aIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("ADD", b.mem);
      }
    } else if (a.type == "ARR" && b.type == "ARR") {
      if (aIndex.type == "NUM" && bIndex.type == "NUM") {
        long long int addrA = a.mem + (stoll(aIndex.name) - a.begin);
        long long int addrB = b.mem + (stoll(bIndex.name) - b.begin);
        memToRegister(addrA);
        if (a.name == b.name && addrA == addrB)
          pushCommand("SHIFT 2");
        else
          pushCommandOneArg("ADD", addrB);
        removeIdentifier(aIndex.name);
        removeIdentifier(bIndex.name);
      } else if (aIndex.type == "NUM" && bIndex.type == "IDE") {
        long long int addrA = a.mem + (stoll(aIndex.name) - a.begin);
        setRegister(to_string((b.mem - b.begin)));
        pushCommandOneArg("ADD", bIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("ADD", addrA);
        removeIdentifier(aIndex.name);
      } else if (aIndex.type == "IDE" && bIndex.type == "NUM") {
        long long int addrB = b.mem + (stoll(bIndex.name) - b.begin);
        setRegister(to_string((a.mem - a.begin)));
        pushCommandOneArg("ADD", aIndex.mem);
        pushCommandOneArg("LOADI", 0);
        pushCommandOneArg("ADD", addrB);
        removeIdentifier(bIndex.name);
      } else if (aIndex.type == "IDE" && bIndex.type == "IDE") {
        if (a.name == b.name && aIndex.name == bIndex.name) {
          setRegister(to_string((a.mem - a.begin)));
          pushCommandOneArg("ADD", aIndex.mem);
          pushCommandOneArg("LOADI", 0);
          pushCommand("SHIFT 2");
        } else {
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

