//
// Created by kolzuk on 1/24/25.
//

#include <VirtualMachine/VirtualMachine.h>

VirtualMachine::VirtualMachine(int64_t heapSize, const Bytecode& bytecode) : heap(heapSize) {
  int64_t currentPos = 0;
  std::string lastFunctionName;
  for (auto& [op, operands] : bytecode ) {
    if (op == FUN_BEGIN) {
      std::string functionName = operands[0];
      std::vector<std::pair<std::string, ValueType>> params;
      for (int i = 1; i < operands.size(); i += 2) {
        ValueType type;
        if (operands[i] == "integer")
          type = INTEGER;
        else if (operands[i] == "array")
          type = ARRAY;

        params.emplace_back(operands[i + 1], type);
      }
      FunctionContext fc;
      fc.paramsDeclaration = params;
      fc.functionName = functionName;
      functionTable[functionName] = fc;
      lastFunctionName = functionName;

      currentPos = 0;
    } else if (op == LABEL) {
      const std::string& labelName = operands[0];
      functionTable[lastFunctionName].labels[labelName] = currentPos;
    }
    functionTable[lastFunctionName].bytecode.emplace_back(op, operands);
    currentPos++;
  }

  if (functionTable.find("main") == functionTable.end()) {
    std::cerr << "Function \"main\" doesn't exist" << std::endl;
    returnCode = -1;
    return;
  }

  StackFrame stackFrame;
  stackFrame.functionContext = functionTable["main"];
  callStack.push(stackFrame);
}

void VirtualMachine::Execute() {
  while (!callStack.empty()) {
    int64_t currentLine = callStack.top().currentPos++;
    auto& command = callStack.top().functionContext.bytecode[currentLine];
    auto& operation = command.first;
    auto& operands = command.second;
    switch (operation) {
      case (ADD): Add(operands); break;
      case (SUB): Sub(operands); break;
      case (MUL): Mul(operands); break;
      case (DIV): Div(operands); break;
      case (MOD): Mod(operands); break;
      case (PUSH): Push(operands); break;

      case (INTEGER_LOAD): IntegerLoad(operands); break;
      case (ARRAY_LOAD): ArrayLoad(operands); break;
      case (LOAD_FROM_INDEX): LoadFromIndex(operands); break;
      case (INTEGER_STORE): IntegerStore(operands); break;
      case (ARRAY_STORE): ArrayStore(operands); break;
      case (STORE_IN_INDEX): StoreInIndex(operands); break;
      case (NEW_ARRAY): NewArray(operands); break;
      case (PRINT): Print(operands); break;

      case (CMP): Cmp(operands); break;
      case (JUMP): Jump(operands); break;
      case (JUMP_EQ): JumpEQ(operands); break;
      case (JUMP_NE): JumpNE(operands); break;
      case (JUMP_LT): JumpLT(operands); break;
      case (JUMP_LE): JumpLE(operands); break;
      case (JUMP_GT): JumpGT(operands); break;
      case (JUMP_GE): JumpGE(operands); break;
      case (RETURN): Return(operands); break;

      case (FUN_CALL): CallFunction(operands); break;
      case (FUN_BEGIN): break;
      case (FUN_END): break;
      case (LABEL): break;
    }
  }
}

void VirtualMachine::Add(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t first = operandStack.top();
  operandStack.pop();
  int64_t second = operandStack.top();
  operandStack.pop();

  operandStack.push(second + first);
}

void VirtualMachine::Sub(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t first = operandStack.top();
  operandStack.pop();
  int64_t second = operandStack.top();
  operandStack.pop();

  operandStack.push(second - first);
}

void VirtualMachine::Mul(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t first = operandStack.top();
  operandStack.pop();
  int64_t second = operandStack.top();
  operandStack.pop();

  operandStack.push(second * first);
}

void VirtualMachine::Div(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t first = operandStack.top();
  operandStack.pop();
  int64_t second = operandStack.top();
  operandStack.pop();

  operandStack.push(second / first);
}

void VirtualMachine::Mod(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t first = operandStack.top();
  operandStack.pop();
  int64_t second = operandStack.top();
  operandStack.pop();

  operandStack.push(second % first);
}

void VirtualMachine::Push(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t value = std::stoll(operands[0]);
  operandStack.push(value);
}

void VirtualMachine::IntegerLoad(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  const std::string& variableName = operands[0];
  if (currentStackFrame.integerVariables.find(variableName) == currentStackFrame.integerVariables.end()) {
    std::cerr << "Integer variable in function: " << currentStackFrame.functionContext.functionName
      << " not found: " << variableName << std::endl;
    return;
  }

  int64_t value = currentStackFrame.integerVariables[variableName];

  operandStack.push(value);
}

void VirtualMachine::ArrayLoad(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string arrayName = operands[0];
  if (currentStackFrame.arrayVariables.find(arrayName) == currentStackFrame.arrayVariables.end()) {
    std::cerr << "Array variable in function: " << currentStackFrame.functionContext.functionName
      << " not found: " << arrayName << std::endl;
    return;
  }

  int64_t value = currentStackFrame.arrayVariables[arrayName];
  operandStack.push(value);
}

void VirtualMachine::LoadFromIndex(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string arrayName = operands[0];
  if (currentStackFrame.arrayVariables.find(arrayName) == currentStackFrame.arrayVariables.end()) {
    std::cerr << "Array variable in function: " << currentStackFrame.functionContext.functionName
      << " not found: " << arrayName << std::endl;
    return;
  }
  int64_t index = operandStack.top();
  operandStack.pop();
  int64_t pointer = currentStackFrame.arrayVariables[arrayName];
  operandStack.push(heap.GetValueByIndex(pointer + index));
}

void VirtualMachine::IntegerStore(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string variableName = operands[0];
  int64_t value = operandStack.top();
  operandStack.pop();
  currentStackFrame.integerVariables[variableName] = value;
}

void VirtualMachine::ArrayStore(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string variableName = operands[0];
  int64_t value = operandStack.top();
  operandStack.pop();
  currentStackFrame.arrayVariables[variableName] = value;
}

void VirtualMachine::StoreInIndex(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string variableName = operands[0];
  int64_t pointer = currentStackFrame.arrayVariables[variableName];
  int64_t index = operandStack.top();
  operandStack.pop();
  int64_t value = operandStack.top();
  operandStack.pop();

  heap.SetValueByIndex(pointer + index, value);
}

void VirtualMachine::Cmp(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t lhs = operandStack.top();
  operandStack.pop();
  int64_t rhs = operandStack.top();
  operandStack.pop();

  compareResult.clear();
  if (lhs == rhs)
    compareResult.EQ = true;
  if (lhs != rhs)
    compareResult.NE = true;
  if (lhs < rhs)
    compareResult.LT = true;
  if (lhs <= rhs)
    compareResult.LE = true;
  if (lhs > rhs)
    compareResult.GT = true;
  if (lhs >= rhs)
    compareResult.GE = true;
}

void VirtualMachine::Jump(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  const std::string& label = operands[0];
  callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
}

void VirtualMachine::JumpEQ(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.EQ) {
    callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpNE(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.NE) {
    callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpLT(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.LT) {
    callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpLE(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.LE) {
    callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpGT(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.GT) {
    callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpGE(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.GE) {
    callStack.top().currentPos = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::NewArray(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t arraySize = operandStack.top();
  operandStack.pop();
  operandStack.push(heap.AllocateMemory(arraySize));
}

void VirtualMachine::Print(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  int64_t value = currentStackFrame.operandStack.top();
  currentStackFrame.operandStack.pop();
  std::cout << value << ' ';
}

void VirtualMachine::CallFunction(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  StackFrame newStackFrame;

  std::string functionName = operands[0];
  auto& params = functionTable[functionName].paramsDeclaration;
  for (auto& param : params) {
    if (param.second == INTEGER)
      newStackFrame.integerVariables[param.first] = currentStackFrame.operandStack.top();
    else if (param.second == ARRAY)
      newStackFrame.arrayVariables[param.first] = currentStackFrame.operandStack.top();

    currentStackFrame.operandStack.pop();
  }

  newStackFrame.functionContext = functionTable[functionName];
  callStack.push(newStackFrame);
}

void VirtualMachine::Return(std::vector<std::string>& operands) {
  auto currentStackFrame = callStack.top();
  int64_t returnedValue = currentStackFrame.operandStack.top();
  callStack.pop();

  if (currentStackFrame.functionContext.functionName == "main") {
    returnCode = returnedValue;
    return;
  }

  if (!callStack.empty()) {
    callStack.top().operandStack.push(returnedValue);
  }
}
