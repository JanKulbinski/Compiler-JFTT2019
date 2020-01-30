#include "compiler.hpp"

void modulo() {

  Identifier a = identifierMap.at(expressionArguments[0]);
  Identifier b = identifierMap.at(expressionArguments[1]);

  Identifier aI, bI;
  if (identifierMap.count(argumentsTabIndex[0]) > 0)
    aI = identifierMap.at(argumentsTabIndex[0]);
  if (identifierMap.count(argumentsTabIndex[1]) > 0)
    bI = identifierMap.at(argumentsTabIndex[1]);

  if (a.type == "NUM" && stoll(a.name) == 0) {
    zeroRegister();
  } else if (b.type == "NUM" && stoll(b.name) == 0) {
    zeroRegister();
  } else if (a.type == "NUM" && b.type == "NUM") {
    long long int valM = abs(stoll(a.name)) % abs(stoll(b.name));
    long long int valA = stoll(a.name);
    long long int valB = stoll(b.name);
    if (valA > 0) {
        if (valB > 0)
    		setRegister(to_string(valM));
    	  else 
    		setRegister(to_string(valM + valB));
    }
    else {
    	 if (valB > 0)
    		setRegister(to_string(valB - valM));
    	  else 
    		setRegister(to_string(-valM));
    }		
    removeIdentifier(a.name);
    removeIdentifier(b.name);

  } else {
    zeroRegister();
    registerToMem(7);
    registerToMem(4);
    registerToMem(8);
    registerToMem(13);
    setToTempMem(a, aI, 5);
    setToTempMem(b, bI, 6);
    registerToMem(14);
    
    //if (b < 0) b = -b
    pushCommandOneArg("JNEG", codeVector.size() + 4);    
    pushCommandOneArg("JPOS", codeVector.size() + 10);
	 zeroRegister();
	 pushCommandOneArg("JUMP", codeVector.size() + 88);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(6);
    
    pushCommandOneArg("SUB", 3);
    registerToMem(6);
    memToRegister(2);
    registerToMem(13);


    //if (a < 0) a = -a
    memToRegister(5);
    pushCommandOneArg("JNEG", codeVector.size() + 4);    
    pushCommandOneArg("JPOS", codeVector.size() + 10);
	 zeroRegister();
	 pushCommandOneArg("JUMP", codeVector.size() + 76);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(5);
    pushCommandOneArg("SUB", 3);
    registerToMem(5);
	 memToRegister(2);
    registerToMem(8);

    //R = a
    memToRegister(5);
    registerToMem(10);		
    registerToMem(11);

    //compute n = #bits of a
    memToRegister(11);
    pushCommandOneArg("SHIFT", 9);
    pushCommandOneArg("JZERO", codeVector.size() + 6);
    registerToMem(11);
    memToRegister(4);
    pushCommand("INC");
    registerToMem(4);
    pushCommandOneArg("JUMP", codeVector.size() - 7);

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
    int stackJ1 = codeVector.size();
    pushCommandOneArg("JNEG", codeVector.size() + 23);
    registerToMem(4);

    // R = R*2 - D
    memToRegister(10);
    pushCommandOneArg("SHIFT", 2);
    pushCommandOneArg("SUB", 11);
    registerToMem(10);

    // if R >= 0
    pushCommandOneArg("JNEG", codeVector.size() + 8);
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

    //R = R / 2^(#bits of a)
    memToRegister(10);
    pushCommandOneArg("JZERO",codeVector.size() + 19);
    
    pushCommandOneArg("SHIFT", 12);
    registerToMem(10);
   	 
    memToRegister(8);
    pushCommandOneArg("JZERO",codeVector.size() + 9);  // a > 0
    
    memToRegister(13);									
    pushCommandOneArg("JZERO", codeVector.size() + 4); // a < 0
    
    zeroRegister();								// a < 0 b < 0
    pushCommandOneArg("SUB", 10);
    pushCommandOneArg("JUMP",codeVector.size() + 10);
    
    memToRegister(14); 							// a < 0 b > 0					
    pushCommandOneArg("SUB", 10);
    pushCommandOneArg("JUMP",codeVector.size() + 7);
    		
    memToRegister(13);							// a > 0 b < 0	
    pushCommandOneArg("JZERO", codeVector.size() + 4);
    								
    memToRegister(10);
    pushCommandOneArg("ADD", 14);
    pushCommandOneArg("JUMP",codeVector.size() + 2);
            								
    memToRegister(10);							// a > 0 b > 0
  }

  argumentsTabIndex[0] = "null";
  argumentsTabIndex[1] = "null";
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
}

void multiply() {

  Identifier a = identifierMap.at(expressionArguments[0]);
  Identifier b = identifierMap.at(expressionArguments[1]);

  Identifier aI, bI;
  if (identifierMap.count(argumentsTabIndex[0]) > 0)
    aI = identifierMap.at(argumentsTabIndex[0]);
  if (identifierMap.count(argumentsTabIndex[1]) > 0)
    bI = identifierMap.at(argumentsTabIndex[1]);

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
  
    zeroRegister();
    registerToMem(7);
    pushCommand("DEC");
    registerToMem(8);
    setToTempMem(a, aI, 5);
    setToTempMem(b, bI, 6);
	 	 
    //if (b < 0) b = -b         	
    pushCommandOneArg("JPOS", codeVector.size() + 8);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(6);
    pushCommandOneArg("SUB", 3);
    registerToMem(6);
    zeroRegister();
    registerToMem(8);

    //if (a < 0) a = -a
    memToRegister(5);
    pushCommandOneArg("JPOS", codeVector.size() + 10);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(5);
    pushCommandOneArg("SUB", 3);
    registerToMem(5);
    memToRegister(8);
    pushCommand("INC");
    registerToMem(8);
    memToRegister(5);

	 // if a > b swap(a,b)
	 pushCommand("SUB 6");
	 pushCommandOneArg("JNEG", codeVector.size() + 8);
	 pushCommandOneArg("JZERO", codeVector.size() + 7);
    memToRegister(6);
    registerToMem(3);
    memToRegister(5);
    registerToMem(6);
    memToRegister(3);
    registerToMem(5);
   
    memToRegister(5);
    
    // a * b
    int stackJ = codeVector.size();
    pushCommandOneArg("JZERO", codeVector.size() + 16);
    pushCommandOneArg("SHIFT", 9);
    pushCommandOneArg("SHIFT", 2);
    pushCommandOneArg("SUB", 5);
    pushCommandOneArg("JNEG", codeVector.size() + 2);
    pushCommandOneArg("JUMP", codeVector.size() + 4);
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
    pushCommandOneArg("JNEG", codeVector.size() + 8);
    pushCommandOneArg("JPOS", codeVector.size() + 7);
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(7);
    pushCommandOneArg("SUB", 3);
    pushCommandOneArg("JUMP", codeVector.size() + 2);
    memToRegister(7);
  }
  argumentsTabIndex[0] = "null";
  argumentsTabIndex[1] = "null";
  expressionArguments[0] = "null";
  expressionArguments[1] = "null";
}

void divide() {

  Identifier a = identifierMap.at(expressionArguments[0]);
  Identifier b = identifierMap.at(expressionArguments[1]);

  Identifier aI, bI;
  if (identifierMap.count(argumentsTabIndex[0]) > 0)
    aI = identifierMap.at(argumentsTabIndex[0]);
  if (identifierMap.count(argumentsTabIndex[1]) > 0)
    bI = identifierMap.at(argumentsTabIndex[1]);

  if (a.type == "NUM" && stoll(a.name) == 0) {
    zeroRegister();
  } else if (b.type == "NUM" && stoll(b.name) == 0) {
    zeroRegister();
  } else if (a.type == "NUM" && b.type == "NUM") {
    long long int val = stoll(a.name) / stoll(b.name);
    if(val >= 0 || (val < 0 && stoll(a.name)%stoll(b.name) == 0))
    	setRegister(to_string(val));
    else 
    	setRegister(to_string(val - 1));

    
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
    pushCommandOneArg("JNEG", codeVector.size() + 4);    
    pushCommandOneArg("JPOS", codeVector.size() + 10);
	 zeroRegister();
	 int divZeroEnd = codeVector.size();
	 pushCommand("JUMP");
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(6);
    pushCommandOneArg("SUB", 3);
    registerToMem(6);
    zeroRegister();
    registerToMem(8);

    //if (a < 0) a = -a
    memToRegister(5);
    pushCommandOneArg("JNEG", codeVector.size() + 4);    
    pushCommandOneArg("JPOS", codeVector.size() + 11);
	 zeroRegister();
	 int divZeroEnd2 = codeVector.size();
	 pushCommand("JUMP");
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(5);
    pushCommandOneArg("SUB", 3);
    registerToMem(5);
    memToRegister(8);
    pushCommand("INC");
    registerToMem(8);
    memToRegister(5);

    // if(|a| < |b|)
    pushCommandOneArg("SUB", 6);
    pushCommandOneArg("JPOS", codeVector.size() + 17);	// |a| > |b|
    pushCommandOneArg("JZERO", codeVector.size() + 8); // |a| = |b|
    memToRegister(8);											// |a| < |b|
    pushCommandOneArg("JPOS", codeVector.size() + 4);
    pushCommandOneArg("JNEG", codeVector.size() + 3);								
    pushCommand("DEC");
    pushCommandOneArg("JUMP", codeVector.size() + 2);
    zeroRegister();
    pushCommandOneArg("JUMP", codeVector.size() + 66);
    
    memToRegister(8);											// |a| = |b|
    pushCommandOneArg("JPOS", codeVector.size() + 4);
    pushCommandOneArg("JNEG", codeVector.size() + 3);	
    pushCommand("DEC");
    pushCommandOneArg("JUMP", codeVector.size() + 3);
    zeroRegister();
    pushCommand("INC");
    pushCommandOneArg("JUMP", codeVector.size() + 58);
    
    //R = a
    memToRegister(5);
    registerToMem(10);
    registerToMem(11);

    //compute n = #bits of a
    memToRegister(11);
    pushCommandOneArg("SHIFT", 9);
    pushCommandOneArg("JZERO", codeVector.size() + 6);
    registerToMem(11);
    memToRegister(4);
    pushCommand("INC");
    registerToMem(4);
    pushCommandOneArg("JUMP", codeVector.size() - 7);

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
    int stackJ1 = codeVector.size();
    pushCommandOneArg("JNEG", codeVector.size() + 23);
    registerToMem(4);

    // R = R*2 - D
    memToRegister(10);
    pushCommandOneArg("SHIFT", 2);
    pushCommandOneArg("SUB", 11);
    registerToMem(10);

    // if R >= 0
    pushCommandOneArg("JNEG", codeVector.size() + 8);
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
    pushCommandOneArg("JNEG", codeVector.size() + 13); // a > 0 and b > 0
    pushCommandOneArg("JPOS", codeVector.size() + 12); // a < 0 and b < 0
    memToRegister(7);
    pushCommandOneArg("SHIFT", 2);
    registerToMem(3);
    memToRegister(7);
    pushCommandOneArg("SUB", 3);
    registerToMem(7);
    memToRegister(10);
    pushCommandOneArg("JZERO", codeVector.size() + 4);
    memToRegister(7);
    pushCommand("DEC");
    pushCommandOneArg("JUMP", codeVector.size() + 2);
    
    memToRegister(7);
    addInt(divZeroEnd, codeVector.size());
    addInt(divZeroEnd2, codeVector.size());
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
	
  Identifier a = identifierMap.at(expressionArguments[0]);
  Identifier b = identifierMap.at(expressionArguments[1]);
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
    if (identifierMap.count(argumentsTabIndex[0]) > 0)
      aIndex = identifierMap.at(argumentsTabIndex[0]);
    if (identifierMap.count(argumentsTabIndex[1]) > 0)
      bIndex = identifierMap.at(argumentsTabIndex[1]);
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

  Identifier a = identifierMap.at(expressionArguments[0]);
  Identifier b = identifierMap.at(expressionArguments[1]);
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
    if (identifierMap.count(argumentsTabIndex[0]) > 0)
      aIndex = identifierMap.at(argumentsTabIndex[0]);
    if (identifierMap.count(argumentsTabIndex[1]) > 0)
      bIndex = identifierMap.at(argumentsTabIndex[1]);
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

