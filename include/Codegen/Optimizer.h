#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <string>

#include <Bytecode/Bytecode.h>

class Optimizer {
 public:
  static void optimize(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode);
};

#endif //OPTIMIZER_H
