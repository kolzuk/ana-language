//
// Created by kolzuk on 1/24/25.
//

#include <VirtualMachine/VirtualMachine.h>

// Читает входные функции, записывает все соответствующие лейблы в контекст функции
int64_t VirtualMachine::ReadFunctions(std::vector<std::pair<Operation, std::vector<std::string>>>& operations) {
  std::string currentFunction;
  for (int64_t pos = 0; pos < operations.size(); ++pos) {
    auto& [operation, operands] = operations[pos];

    if (operation == FUN_BEGIN) {
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
      fc.pos = pos;
      fc.paramsDeclaration = params;
      fc.functionName = functionName;
      functionTable[functionName] = fc;
      lastFunctionName = functionName;
    } else if (operation == FUN_END) {
    } else if (operation == LABEL) {
      const std::string& labelName = operands[0];
      functionTable[lastFunctionName].labels[labelName] = pos;
    }
  }

  if (functionTable.find("main") == functionTable.end()) {
    std::cerr << "Function \"main\" doesn't exist" << std::endl;
    returnCode = -1;
    return -1;
  }

  StackFrame stackFrame;
  stackFrame.calledPos = 10000000;
  stackFrame.functionContext = functionTable["main"];
  callStack.push(stackFrame);

  return functionTable["main"].pos;
}

void VirtualMachine::Execute(std::vector<std::pair<Operation, std::vector<std::string>>>& operations) {
  currentLine = ReadFunctions(operations) + 1;

  while (currentLine < operations.size()) {
    auto& [operation, operands] = operations[currentLine];
    // 13 -> 59
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

    currentLine++;
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
  currentLine = currentStackFrame.functionContext.labels[label];
}

void VirtualMachine::JumpEQ(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.EQ) {
    currentLine = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpNE(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.NE) {
    currentLine = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpLT(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.LT) {
    currentLine = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpLE(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.LE) {
    currentLine = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpGT(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.GT) {
    currentLine = currentStackFrame.functionContext.labels[label];
  }
}

void VirtualMachine::JumpGE(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
  if (compareResult.GE) {
    currentLine = currentStackFrame.functionContext.labels[label];
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
  newStackFrame.calledPos = currentLine;
  newStackFrame.functionContext = functionTable[functionName];
  callStack.push(newStackFrame);
  currentLine = functionTable[functionName].pos;
}

void VirtualMachine::Return(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  currentLine = callStack.top().calledPos;
  int64_t returnedValue = currentStackFrame.operandStack.top();
  if (currentStackFrame.functionContext.functionName == "main") {
    returnCode = returnedValue;
    return;
  }

  callStack.pop();
  callStack.top().operandStack.push(returnedValue);
}

void VirtualMachine::FunBegin(std::vector<std::string>& operands) {
}

void VirtualMachine::FunEnd(std::vector<std::string>& operands) {
}
