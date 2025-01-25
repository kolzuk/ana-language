#include "Codegen/Optimizer.h"

#include <string>
#include <map>
#include <stack>
#include <set>

void DeadCodeElimination(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  std::set<std::string> markedFunctions;
  std::set<std::string> allFunctions;
  std::map<std::string, std::vector<std::string>> allCalls;
  std::string currentFunction;
  for (auto& [operation, operands] : bytecode) {
    if (operation == FUN_BEGIN) {
      currentFunction = operands[0];
      allFunctions.insert(currentFunction);
    } else if (operation == CALL) {
      std::string functionName = operands[0];
      allCalls[currentFunction].push_back(functionName);
    }
  }

  markedFunctions.insert("main");
  std::stack<std::string> checkFunctions;
  for (int i = 0; i < allCalls["main"].size(); ++i)
    checkFunctions.push(allCalls["main"][i]);

  while (!checkFunctions.empty()) {
    std::string function = checkFunctions.top();
    checkFunctions.pop();
    markedFunctions.insert(function);

    for (auto& childFunction : allCalls[function]) {
      if (markedFunctions.find(childFunction) != markedFunctions.end()) {
        checkFunctions.push(childFunction);
        markedFunctions.insert(childFunction);
      }
    }
  }
}

void Optimizer::optimize(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {

}
