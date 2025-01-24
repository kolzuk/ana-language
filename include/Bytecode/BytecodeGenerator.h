#ifndef BYTECODEGENERATOR_H
#define BYTECODEGENERATOR_H

#include "Parser/AST.h"
#include <vector>

class BytecodeGenerator {

 public:
  char* generate(AST&);
};

#endif //BYTECODEGENERATOR_H
