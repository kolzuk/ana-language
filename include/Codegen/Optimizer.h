#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <string>

#include <Bytecode/Bytecode.h>

class Optimizer {
 public:
  static void optimize(std::vector<std::pair<Operations, std::string>>& Module);
};

#endif //OPTIMIZER_H
