#include "Codegen/Optimizer.h"

#include <string>
#include <map>
#include <queue>
#include <set>
#include <vector>

void DeadCodeElimination(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  std::map<std::string, std::set<std::string>> allCalls;
  std::string currentFunction;
  std::map<std::string, int> usefulness;
  for (auto& [operation, operands] : bytecode) {
    if (operation == FUN_BEGIN) {
      currentFunction = operands[0];
      usefulness[currentFunction] = 0;
    } else if (operation == FUN_CALL) {
      std::string functionName = operands[0];
      allCalls[currentFunction].insert(functionName);
    }
  }

  std::queue<std::string> checkFunctions;
  checkFunctions.push(currentFunction);
  while (!checkFunctions.empty()) {
    currentFunction = checkFunctions.front();
    usefulness[currentFunction]++;
    checkFunctions.pop();
    for (auto& function  : allCalls[currentFunction]) {
      if (usefulness[function] == 0) {
        checkFunctions.push(function);
      }
    }
  }


  bool isUseful = false;
  std::vector<std::pair<Operation, std::vector<std::string>>> optimizedBytecode;
  for (auto& [operation, operands] : bytecode) {
    if (operation == FUN_BEGIN) {
      currentFunction = operands[0];
      isUseful = usefulness[currentFunction] != 0;
    }
    if (isUseful) {
      optimizedBytecode.push_back({operation, operands});
    }
    if (operation == FUN_END) {
      isUseful = false;
    }
  }

  bytecode.swap(optimizedBytecode);
}

void Optimizer::optimize(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  DeadCodeElimination(bytecode);
}
