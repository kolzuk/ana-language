#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm/Support/CommandLine.h>
#include "Parser/AST.h"

class CodeGen {
 public:
  static void compile(AST* Tree, const std::string& SourceFilename);
};

#endif //CODEGEN_H
