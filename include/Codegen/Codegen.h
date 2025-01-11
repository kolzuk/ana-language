#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm/Support/CommandLine.h>
#include "Parser/AST.h"

class CodeGen {
 public:
  static void compile(AST* Tree, std::string& sourceFilename);
};

#endif //CODEGEN_H
