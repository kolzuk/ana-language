//
// Created by kolzuk on 1/24/25.
//

#include <VirtualMachine/VirtualMachine.h>

void VirtualMachine::execute(std::vector<std::pair<Operations, std::vector<std::string>>>& operations) {
  for (auto& line : operations) {
    Operations op = line.first;

    switch (op) {
      
    }
  }
}
