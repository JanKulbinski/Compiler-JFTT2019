#include "compiler.hpp"

void ifEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JPOS");
  Jump jump2;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JNEG");
}

void ifNotEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JZERO");
}

void ifLess() {
  subtract();

  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JZERO");
  Jump jump2;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JPOS");
}

void ifGreater() {
  subtract();

  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JZERO");
  Jump jump2;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JNEG");
}

void ifLessEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JPOS");
}

void ifGreaterEqual() {
  subtract();

  Jump jump;
  createJump( & jump, codeVector.size(), depth);
  jumpVector.push_back(jump);
  pushCommand("JNEG");
}
