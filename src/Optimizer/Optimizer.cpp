#include "Optimizer/Optimizer.h"

#include <string>
#include <map>
#include <queue>
#include <set>
#include <vector>

static std::map<Operation, int> operationsStackUsage = {
    {ADD, 2},
    {SUB, 2},
    {MUL, 2},
    {DIV, 2},
    {MOD, 2},
    {PUSH, 0},
    {INTEGER_LOAD, 0},
    {ARRAY_LOAD, 0},
    {LOAD_FROM_INDEX, 1},
    {INTEGER_STORE, 1},
    {ARRAY_STORE, 1},
    {STORE_IN_INDEX, 1},
    {NEW_ARRAY, 1},
    {PRINT, 1},
    {FUN_BEGIN, 0},
    {FUN_END, 0},
    {FUN_CALL, 0},
    {RETURN, 1},
    {LABEL, 1},
    {JUMP, 0},
    {CMP, 2},
    {JUMP_EQ, 0},
    {JUMP_NE, 0},
    {JUMP_LT, 0},
    {JUMP_LE, 0},
    {JUMP_GT, 0},
    {JUMP_GE, 0}
};

bool VariableStoringElimination(
    std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode,
    std::vector<bool>& mask,
    size_t startIndex)
{
  bool isEliminated = false;
  size_t opIndex = startIndex;
  Operation operation = bytecode[opIndex].first;
  size_t remainingCount = 1;
  while (remainingCount > 0) {
    --remainingCount;
    remainingCount += operationsStackUsage[operation];
    if (mask[opIndex]) {
      isEliminated = true;
      mask[opIndex] = false;
    }

    --opIndex;
    operation = bytecode[opIndex].first;
  }

  return isEliminated;
}


bool VariableEliminationInsideFunction(
    std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode,
    std::vector<bool>& mask,
    size_t begin,
    size_t end) {
  bool isEliminated = false;

  std::string currentVariable;
  std::map<std::string, bool> uselessness;
  for (size_t opIndex = begin; opIndex < end; opIndex++) {
    auto& [operation, operands] = bytecode[opIndex];
    if (operation == INTEGER_STORE || operation == ARRAY_STORE || operation == STORE_IN_INDEX) {
      currentVariable = operands[0];
      if (mask[opIndex]) {
        uselessness[currentVariable] = true;
      }
    }
  }

  for (auto& [variable, isUseless] : uselessness) {
    for (size_t opIndex = begin; opIndex < end; opIndex++) {
      auto& [operation, operands] = bytecode[opIndex];
      if (operation == INTEGER_LOAD || operation == ARRAY_LOAD || operation == LOAD_FROM_INDEX) {
        currentVariable = operands[0];
        if ((currentVariable == variable) && mask[opIndex]) {
          uselessness[currentVariable] = false;
          break;
        }
      }
    }
  }

  for (auto& [variable, isUseless] : uselessness) {
    if (!uselessness[variable]) {
      continue;
    }
    for (size_t opIndex = begin; opIndex < end; opIndex++) {
      auto& [operation, operands] = bytecode[opIndex];
      if (operation == INTEGER_STORE || operation == ARRAY_STORE || operation == STORE_IN_INDEX) {
        currentVariable = operands[0];
        if ((currentVariable == variable) && mask[opIndex]) {
          isEliminated = isEliminated || VariableStoringElimination(bytecode, mask, opIndex);
        }
      }
    }
  }

  return isEliminated;
}

void VariableElimination(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode, std::vector<bool>& mask) {
  std::string currentFunction;
  size_t funcBeginIndex = 0;
  bool isEliminated = true;
  while (isEliminated) {
    isEliminated = false;
    for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
      auto& [operation, operands] = bytecode[opIndex];
      if (operation == FUN_BEGIN && mask[opIndex]) {
        funcBeginIndex = opIndex;
      }
      if (operation == FUN_END && mask[opIndex]) {
        if (mask[funcBeginIndex]) {
          isEliminated = isEliminated || VariableEliminationInsideFunction(bytecode, mask, funcBeginIndex, opIndex);
        }
      }
    }
  }
}

void FunctionElimination(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode, std::vector<bool>& mask) {
  std::map<std::string, std::set<std::string>> allCalls;
  std::string currentFunction;
  std::map<std::string, int> usefulness;
  for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
    auto& [operation, operands] = bytecode[opIndex];
    if (operation == FUN_BEGIN) {
      currentFunction = operands[0];
      usefulness[currentFunction] = 0;
    } else if (operation == FUN_CALL && mask[opIndex]) {
      std::string functionName = operands[0];
      allCalls[currentFunction].insert(functionName);
    }
  }

  std::queue<std::string> checkFunctions;
  checkFunctions.push("main");
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
  for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
    auto& [operation, operands] = bytecode[opIndex];
    if (operation == FUN_BEGIN) {
      currentFunction = operands[0];
      isUseful = usefulness[currentFunction] != 0;
    }
    if (!isUseful) {
      mask[opIndex] = false;
    }
    if (operation == FUN_END) {
      isUseful = false;
    }
  }

}

void DeadCodeElimination(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  std::vector<bool> mask = std::vector<bool>(bytecode.size(), true);

  VariableElimination(bytecode, mask);

  std::vector<std::pair<Operation, std::vector<std::string>>> optimizedBytecode;
  for (size_t op_index = 0; op_index < bytecode.size(); op_index++) {
    if (mask[op_index]) {
      optimizedBytecode.push_back(bytecode[op_index]);
    }
  }
  bytecode.swap(optimizedBytecode);
}

void Optimizer::optimize(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  DeadCodeElimination(bytecode);
}