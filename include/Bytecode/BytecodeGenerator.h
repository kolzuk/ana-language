#ifndef ANA_LANGUAGE_INCLUDE_BYTECODE_BYTECODEGENERATOR_H_
#define ANA_LANGUAGE_INCLUDE_BYTECODE_BYTECODEGENERATOR_H_

#include "Parser/AST.h"
#include <vector>

class BytecodeGenerator {

 public:
  char* generate(AST&);
};

#endif //ANA_LANGUAGE_INCLUDE_BYTECODE_BYTECODEGENERATOR_H_
