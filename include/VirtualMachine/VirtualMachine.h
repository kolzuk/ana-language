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

struct FunctionContext {
  std::string functionName;
  std::vector<std::pair<std::string, ValueType>> paramsDeclaration;
  int64_t pos;
};

struct StackFrame {
  std::stack<int64_t> operandStack;
  std::map<std::string, int64_t> integerVariables;
  std::map<std::string, int64_t> arrayVariables;
  int64_t calledPos = -1;
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
  int64_t currentLine;
  bool isFunctionDeclaration = false;
  int64_t returnCode = 0;
  CompareResult compareResult;
 public:
  explicit VirtualMachine(int64_t heapSize) : heap(heapSize), currentLine(0) {}
  void Execute(std::vector<std::pair<Operation, std::vector<std::string>>>& operations);
  [[nodiscard]] int64_t getReturnCode() const { return returnCode; }

  void Add(std::vector<std::string>& operands);
  void Sub(std::vector<std::string>& operands);
  void Mul(std::vector<std::string>& operands);
  void Div(std::vector<std::string>& operands);
  void Mod(std::vector<std::string>& operands);

  void Push(std::vector<std::string>& operands);
  void Load(std::vector<std::string>& operands);
  void LoadFromIndex(std::vector<std::string>& operands);
  void Store(std::vector<std::string>& operands);
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

  void FunBegin(std::vector<std::string>& operands);
  void FunEnd(std::vector<std::string>& operands);
};

#endif //VIRTUAL_MACHINE_H
