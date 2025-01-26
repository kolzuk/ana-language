//
// Created by kolzuk on 1/24/25.
//

#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <Bytecode/Bytecode.h>
#include <VirtualMachine/Heap.h>
#include <Optimizer/Optimizer.h>

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <iostream>
#include <memory>
#include <set>

class VirtualMachine;

class GarbageCollector {
  std::weak_ptr<VirtualMachine> vm;
 public:
  explicit GarbageCollector(const std::shared_ptr<VirtualMachine>& vm) : vm(vm) {}

  void CollectGarbage();
};

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
  FunctionContext functionContext;
  int64_t currentPos = 0;
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

struct ProfilingContext {
  std::set<std::string> optimizedFunctions;
  std::map<std::string, int64_t> functionCalls;
  int64_t callThreshold = 1000;
};

class VirtualMachine : public std::enable_shared_from_this<VirtualMachine>  {
  std::shared_ptr<GarbageCollector> garbageCollector;
  Heap heap;
  std::map<std::string, FunctionContext> functionTable;
  std::vector<StackFrame> callStack;
  int64_t returnCode = 0;
  CompareResult compareResult;
  ProfilingContext profilingContext;
  friend class GarbageCollector;
 public:
  VirtualMachine(int64_t heapSize, const Bytecode& bytecode);
  void Execute();
  void EmergencyTermination();
  [[nodiscard]] int64_t getReturnCode() const { return returnCode; }
  void InitializeGarbageCollector() {
    garbageCollector = std::make_shared<GarbageCollector>(shared_from_this());
  }

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
