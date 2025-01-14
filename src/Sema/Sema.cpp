#include "Sema/Sema.h"
#include "Parser/AST.h"

bool Sema::semantic(AST* Tree) {
  if (!Tree)
    return false;
  Tree->accept(Visitor);
  return !Visitor.hasError();
}

