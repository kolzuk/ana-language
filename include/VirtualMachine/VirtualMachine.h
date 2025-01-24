//
// Created by kolzuk on 1/24/25.
//

#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <Bytecode/Bytecode.h>

#include <vector>
#include <string>

class VirtualMachine {
 public:
  void execute(std::vector<std::pair<Operations, std::vector<std::string>>>& operations);
};

#endif //VIRTUAL_MACHINE_H
