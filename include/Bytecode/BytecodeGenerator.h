#ifndef BYTECODEGENERATOR_H
#define BYTECODEGENERATOR_H

#include "Bytecode/Bytecode.h"
#include "Parser/AST.h"
#include <vector>

class BytecodeGenerator {

 public:
  std::vector<std::pair<Operation, std::vector<std::string>>> generate(AST&);
};

#endif //BYTECODEGENERATOR_H