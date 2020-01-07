#include "compiler.hpp"

void ifEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JPOS");
  Jump jump2;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JNEG");
}

void ifNotEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JZERO");
}

void ifLess() {
  subtract();

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JZERO");
  Jump jump2;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JPOS");
}

void ifGreater() {
  subtract();

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JZERO");
  Jump jump2;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JNEG");
}

void ifLessEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JPOS");
}

void ifGreaterEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeStack.size(), depth);
  jumpStack.push_back(jump);
  pushCommand("JNEG");
}
