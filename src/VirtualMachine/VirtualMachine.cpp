//
// Created by kolzuk on 1/24/25.
//

#include <VirtualMachine/VirtualMachine.h>

void VirtualMachine::Execute(std::vector<std::pair<Operation, std::vector<std::string>>>& operations) {
  currentLine = 0;

  while (currentLine < operations.size()) {
    auto& [operation, operands] = operations[currentLine];
    if (isFunctionDeclaration && operation != FUN_BEGIN && operation != FUN_END && operation != LABEL) {
      currentLine++;
      continue;
    }

    switch (operation) {
      case (FUN_BEGIN): FunBegin(operands); break;
      case (FUN_END): FunEnd(operands); break;
      case (ADD): Add(operands); break;
      case (SUB): Sub(operands); break;
      case (MUL): Mul(operands); break;
      case (DIV): Div(operands); break;
      case (MOD): Mod(operands); break;

      case (PUSH): Push(operands); break;
      case (LOAD): Load(operands); break;
      case (ARRAY_LOAD): ArrayLoad(operands); break;
      case (LOAD_FROM_INDEX): LoadFromIndex(operands); break;
      case (STORE): Store(operands); break;
      case (STORE_IN_INDEX): StoreInIndex(operands); break;

      case CMP: Cmp(operands); break;
      case LABEL: Label(operands); break;
      case JUMP: Jump(operands); break;
      case JUMP_EQ: JumpEQ(operands); break;
      case JUMP_NE: JumpNE(operands); break;
      case JUMP_LT: JumpLT(operands); break;
      case JUMP_LE: JumpLE(operands); break;
      case JUMP_GT: JumpGT(operands); break;
      case JUMP_GE: JumpGE(operands); break;

      case (NEW_ARRAY): NewArray(operands); break;
      case (PRINT): Print(operands); break;
      case (CALL): CallFunction(operands); break;
      case (RETURN): Return(operands); break;
    }

    currentLine++;
  }
}

void VirtualMachine::Add(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t lhs = operandStack.top();
  operandStack.pop();
  int64_t rhs = operandStack.top();
  operandStack.pop();

  operandStack.push(lhs + rhs);
}

void VirtualMachine::Sub(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t lhs = operandStack.top();
  operandStack.pop();
  int64_t rhs = operandStack.top();
  operandStack.pop();

  operandStack.push(lhs - rhs);
}

void VirtualMachine::Mul(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t lhs = operandStack.top();
  operandStack.pop();
  int64_t rhs = operandStack.top();
  operandStack.pop();

  operandStack.push(lhs * rhs);
}

void VirtualMachine::Div(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t lhs = operandStack.top();
  operandStack.pop();
  int64_t rhs = operandStack.top();
  operandStack.pop();

  operandStack.push(lhs / rhs);
}

void VirtualMachine::Mod(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t lhs = operandStack.top();
  operandStack.pop();
  int64_t rhs = operandStack.top();
  operandStack.pop();

  operandStack.push(lhs % rhs);
}

void VirtualMachine::Push(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  int64_t value = std::stoll(operands[0]);
  operandStack.push(value);
}

void VirtualMachine::Load(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string variableName = operands[0];
  if (currentStackFrame.integerVariables.find(variableName) == currentStackFrame.integerVariables.end()) {
    std::cerr << "Integer variable in function: " << currentStackFrame.functionContext.functionName << " not found: " << variableName;
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
    std::cerr << "Array variable in function: " << currentStackFrame.functionContext.functionName << " not found: " << arrayName;
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
    std::cerr << "Array variable in function: " << currentStackFrame.functionContext.functionName << " not found: " << arrayName;
    return;
  }
  int64_t index = operandStack.top();
  operandStack.pop();
  int64_t pointer = currentStackFrame.arrayVariables[arrayName];
  operandStack.push(heap.GetValueByIndex(pointer + index));
}

void VirtualMachine::Store(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  auto& operandStack = currentStackFrame.operandStack;

  std::string variableName = operands[0];
  int64_t value = operandStack.top();
  operandStack.pop();
  currentStackFrame.integerVariables[variableName] = value;
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

void VirtualMachine::Label(std::vector<std::string>& operands) {
  if (!isFunctionDeclaration) {
    return;
  }

  std::string labelName = operands[0];
  functionTable[lastFunctionName].labels[labelName] = currentLine;
}

void VirtualMachine::Jump(std::vector<std::string>& operands) {
  auto& currentStackFrame = callStack.top();
  std::string label = operands[0];
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

  std::string arrayName = operands[0];
  int64_t arraySize = operandStack.top();
  operandStack.pop();

  currentStackFrame.arrayVariables[arrayName] = heap.AllocateMemory(arraySize);
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
  fc.pos = currentLine;
  fc.paramsDeclaration = params;
  fc.functionName = functionName;
  functionTable[functionName] = fc;
  lastFunctionName = functionName;

  if (functionName == "main") {
    StackFrame stackFrame;
    stackFrame.calledPos = 10000000;
    stackFrame.functionContext = fc;
    callStack.push(stackFrame);
    isFunctionDeclaration = false;
  } else {
    isFunctionDeclaration = true;
  }
}

void VirtualMachine::FunEnd(std::vector<std::string>& operands) {
  isFunctionDeclaration = false;
}
