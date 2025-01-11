#ifndef CODEGEN_H
#define CODEGEN_H

#include "Parser/AST.h"

class CodeGen {
 public:
  static void compile(AST *Tree);
};

#endif //CODEGEN_H
