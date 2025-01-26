//
// Created by kolzuk on 1/24/25.
//

#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <Bytecode/Bytecode.h>
#include <VirtualMachine/Heap.h>

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <iostream>

enum ValueType {
  INTEGER,
  ARRAY
};

using Bytecode = std::vector<std::pair<Operation, std::vector<std::string>>>;

struct FunctionContext {
  std::string functionName;
  std::vector<std::pair<std::string, ValueType>> paramsDeclaration;
  std::map<std::string, int64_t> labels;
  Bytecode bytecode;
};

struct StackFrame {
  std::stack<int64_t> operandStack;
  std::map<std::string, int64_t> integerVariables;
  std::map<std::string, int64_t> arrayVariables;
  int64_t currentPos = 0;
  FunctionContext functionContext;
};

struct CompareResult {
  bool EQ; // ==
  bool NE; // !=
  bool LT; // <
  bool LE; // <=
  bool GT; // >
  bool GE; // >=

  void clear() {
    EQ = false; NE = false; LT = false; LE = false; GT = false; GE = false;
  }
};

class VirtualMachine {
  Heap heap;
  std::map<std::string, FunctionContext> functionTable;
  std::stack<StackFrame> callStack;
  int64_t returnCode = 0;
  CompareResult compareResult;
 public:
  VirtualMachine(int64_t heapSize, const Bytecode& bytecode);
  void Execute();
  [[nodiscard]] int64_t getReturnCode() const { return returnCode; }

  void Add(std::vector<std::string>& operands);
  void Sub(std::vector<std::string>& operands);
  void Mul(std::vector<std::string>& operands);
  void Div(std::vector<std::string>& operands);
  void Mod(std::vector<std::string>& operands);

  void Push(std::vector<std::string>& operands);
  void IntegerLoad(std::vector<std::string>& operands);
  void ArrayLoad(std::vector<std::string>& operands);
  void LoadFromIndex(std::vector<std::string>& operands);
  void IntegerStore(std::vector<std::string>& operands);
  void ArrayStore(std::vector<std::string>& operands);
  void StoreInIndex(std::vector<std::string>& operands);

  void Jump(std::vector<std::string>& operands);
  void Cmp(std::vector<std::string>& operands);
  void JumpEQ(std::vector<std::string>& operands);
  void JumpNE(std::vector<std::string>& operands);
  void JumpLT(std::vector<std::string>& operands);
  void JumpLE(std::vector<std::string>& operands);
  void JumpGT(std::vector<std::string>& operands);
  void JumpGE(std::vector<std::string>& operands);


  void NewArray(std::vector<std::string>& operands);
  void Print(std::vector<std::string>& operands);
  void CallFunction(std::vector<std::string>& operands);
  void Return(std::vector<std::string>& operands);
};

#endif //VIRTUAL_MACHINE_H
