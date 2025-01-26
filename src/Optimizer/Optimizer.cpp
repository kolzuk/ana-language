#include "Optimizer/Optimizer.h"

#include <string>
#include <map>
#include <stack>
#include <vector>

static std::map<Operation, int> operationsStackUsage = {
    {ADD, 2}, // +
    {SUB, 2}, // +
    {MUL, 2}, // +
    {DIV, 2}, // +
    {MOD, 2}, // +
    {PUSH, 0},
    {INTEGER_LOAD, 0},
    {ARRAY_LOAD, 0},
    {LOAD_FROM_INDEX, 1},
    {INTEGER_STORE, 1},  // +
    {ARRAY_STORE, 1},  // +
    {STORE_IN_INDEX, 1},  // +
    {NEW_ARRAY, 1},
    {PRINT, 1},  // +
    {FUN_BEGIN, 0},
    {FUN_END, 0},
    {FUN_CALL, 0},
    {RETURN, 1}, // +
    {LABEL, 1},
    {JUMP, 0},
    {CMP, 2}, // +
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
    std::vector<bool>& mask) {
  bool isEliminated = false;

  std::string currentVariable;
  std::map<std::string, bool> uselessness;
  for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
    auto& [operation, operands] = bytecode[opIndex];
    if (operation == INTEGER_STORE || operation == ARRAY_STORE || operation == STORE_IN_INDEX) {
      currentVariable = operands[0];
      if (mask[opIndex]) {
        uselessness[currentVariable] = true;
      }
    }
  }

  for (auto& [variable, isUseless] : uselessness) {
    for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
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
    for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
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

void DeadCodeElimination(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  std::vector<bool> mask = std::vector<bool>(bytecode.size(), true);

  std::string currentFunction;
  bool isEliminated = true;
  while (isEliminated) {
    isEliminated = VariableEliminationInsideFunction(bytecode, mask);
  }

  std::vector<std::pair<Operation, std::vector<std::string>>> optimizedBytecode;
  for (size_t op_index = 0; op_index < bytecode.size(); op_index++) {
    if (mask[op_index]) {
      optimizedBytecode.push_back(bytecode[op_index]);
    }
  }
  bytecode.swap(optimizedBytecode);
}

int64_t Calculating(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode,
    std::vector<bool>& mask,
    size_t topIndex,
    size_t bottomIndex) {
  std::stack<int64_t> stack;
  size_t opIndex = topIndex;
  int64_t lhs;
  int64_t rhs;
  while (opIndex < bottomIndex) {
    auto& [operation, operands] = bytecode[opIndex];
    mask[opIndex] = false;
    switch (operation) {
      case PUSH:
        stack.push(std::stoll(operands[0]));
        break;
      case ADD:
        rhs = stack.top();
        stack.pop();
        lhs = stack.top();
        stack.pop();
        stack.push(lhs + rhs);
        break;
      case SUB:
        rhs = stack.top();
        stack.pop();
        lhs = stack.top();
        stack.pop();
        stack.push(lhs - rhs);
        break;
      case MUL:
        rhs = stack.top();
        stack.pop();
        lhs = stack.top();
        stack.pop();
        stack.push(lhs * rhs);
        break;
      case DIV:
        rhs = stack.top();
        stack.pop();
        lhs = stack.top();
        stack.pop();
        stack.push(lhs / rhs);
        break;
      case MOD:
        rhs = stack.top();
        stack.pop();
        lhs = stack.top();
        stack.pop();
        stack.push(lhs % rhs);
        break;
    }
    ++opIndex;
  }
  return stack.top();
}

bool checkFolding(
    std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode,
    size_t topIndex,
    size_t bottomIndex)
{
  Operation operation;
  for (size_t opIndex = topIndex; opIndex < bottomIndex; opIndex++) {
    operation = bytecode[opIndex].first;
    if (!(operation == ADD || operation == SUB || operation == MUL
    || operation == DIV || operation == MOD || operation == PUSH)) {
      return false;
    }
  }

  return true;
}

bool Folding(
    std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode,
    std::vector<bool>& mask,
    size_t startIndex)
{
  bool toFold = false;
  size_t opIndex;
  size_t bottomIndex = startIndex;
  Operation operation = bytecode[startIndex].first;
  size_t remainingCount;
  int64_t result;

  size_t rounds = 1;
  if (operation == CMP) {
    rounds = 2;
  }
  for (size_t i = 0; i < rounds; ++i) {
    opIndex = bottomIndex;
    remainingCount = 1;
    while (remainingCount > 0) {
      --remainingCount;
      --opIndex;
      operation = bytecode[opIndex].first;
      remainingCount += operationsStackUsage[operation];
    }

    if (checkFolding(bytecode, opIndex, bottomIndex)) {
      toFold = true;
      result = Calculating(bytecode, mask, opIndex, bottomIndex);
      mask[bottomIndex - 1] = true;
      bytecode[bottomIndex - 1] = std::make_pair(PUSH, std::vector<std::string>{std::to_string(result)});
    }

    bottomIndex = opIndex;
  }
  return toFold;
}


bool ConstantFolding(
    std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  std::vector<bool> mask = std::vector<bool>(bytecode.size(), true);
  bool isFolded = false;

  std::string currentVariable;
  std::map<std::string, bool> uselessness;
  for (size_t opIndex = 0; opIndex < bytecode.size(); opIndex++) {
    auto& [operation, operands] = bytecode[opIndex];
    if (operation == INTEGER_STORE || operation == ARRAY_STORE || operation == STORE_IN_INDEX
    || operation == PRINT || operation == RETURN || operation == CMP) {
      isFolded = Folding(bytecode, mask, opIndex);
    }
  }

  std::vector<std::pair<Operation, std::vector<std::string>>> optimizedBytecode;
  for (size_t op_index = 0; op_index < bytecode.size(); op_index++) {
    if (mask[op_index]) {
      optimizedBytecode.push_back(bytecode[op_index]);
    }
  }
  bytecode.swap(optimizedBytecode);

 return isFolded;
}

void Optimizer::optimize(std::vector<std::pair<Operation, std::vector<std::string>>>& bytecode) {
  DeadCodeElimination(bytecode);
  ConstantFolding(bytecode);
}