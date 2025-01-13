#include "Sema/Sema.h"
#include "Parser/AST.h"

bool Sema::semantic(AST* Tree) {
  Tree->accept(Visitor);
  return !Visitor.hasError();
}

