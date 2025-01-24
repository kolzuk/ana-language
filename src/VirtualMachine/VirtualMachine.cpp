//
// Created by kolzuk on 1/24/25.
//

#include <VirtualMachine/VirtualMachine.h>

void VirtualMachine::execute(std::vector<std::pair<Operation, std::vector<std::string>>>& operations) {
  for (auto& line : operations) {
    Operation op = line.first;

    switch (op) {

    }
  }
}
