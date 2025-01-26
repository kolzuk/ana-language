#include "Codegen/Optimizer.h"

#include <string>
#include <map>
#include <queue>
#include <set>
#include <vector>

static std::map<Operation, int> operationsStackUsage = {
  /// Берёт два значения из стека операндов, суммирует их,
  /// и кладёт полученное значение обратно в операндовый стек.
{ADD, 2},

  /// Берёт два значения из стека операндов, вычитает одно значение из другого,
  /// и кладёт полученное значение обратно в операндовый стек.
{SUB, 2},

  /// Берёт два значения из стека операндов, умножает их,
  /// и кладёт полученное значение обратно в операндовый стек.
{MUL, 2},


  /// Берёт два значения из стека операндов, делит одно значение на другое,
  /// и кладёт полученное значение обратно в операндовый стек.
{DIV, 2},

  /// Берёт два значения из стека операндов, берёт одно значение по модулю второго,
  /// и кладёт полученное значение обратно в операндовый стек.
{MOD, 2},

  /// Кладёт значение в операндовый стек (только 8-и байтное значение)
  /// Например: PUSH "8"
{PUSH, 0},

  /// Загружает значение переменной в операндовый стек (только 8-и байтное значение)
  /// Например: INTEGER_LOAD "a"
{INTEGER_LOAD, 0},

  /// Загружает указатель массива в операндовый стек (только 8-и байтное значение)
  /// Например: ARRAY_LOAD "array"
{ARRAY_LOAD, 0},


  /// Загружает значение массива в позиции i в операндовый стек (только 8-и байтное значение)
  /// Например: LOAD_FROM_INDEX "array" 2
  /// Например: LOAD_FROM_INDEX 12, 3
{LOAD_FROM_INDEX, 1},

  /// Загружает в значение переменной значение из операндового стека (только 8-и байтное значение).
  /// Например: INTEGER_STORE "a"
{INTEGER_STORE, 1},

  /// Загружает в указатель массива значение из операндового стека
{ARRAY_STORE, 1},

  /// Загружает в значение элемента массива по индексу
  /// значение из операндового стека (только 8-и байтное значение).
  /// Индекс берёт из стека.
  /// Например: STORE_IN_INDEX "a"
{STORE_IN_INDEX, 1},

  /// Инициализирует массив. Предварительно нужно положить размер массива
  /// в операндовый стек. Затем кладёт указатель на вершину стека.
  /// Например: PUSH 10; NEW ARRAY; // создаёт массив размером 10.
{NEW_ARRAY, 1},

  /// Выводит в std::cout последнее значение из операндового стека.
  /// Например: PUSH 1; PRINT; // std::cout << 10; значение из операндового стека убирается.
{PRINT, 1},


  /// Декларирует функцию следующим.
  /// Например: FUN_BEGIN foo integer a, integer b, array arr
{FUN_BEGIN, 0},

  /// Пометка о том, что функция закончилась.
{FUN_END, 0},

  /// Вызывает указанную функцию, параметры передаются через операндовый стек.
  /// Например: PUSH 1; INTEGER_LOAD a; FUN_CALL SUM;
  /// Код выше эквивалентен соответствующему: SUM(a, 1);
{FUN_CALL, 0},

  /// Возвращает верхнее значение из операндового стека в операндовый стек вызываемого стекфрейма.
  /// Сдвигает текущий указатель исполнения кода на вызвавший его стекфрейма.
  /// Например: PUSH 1; RETURN;
{RETURN, 1},

  /// Создаёт LABEL
  /// Например: LABEL "condition"
{LABEL, 1},

  /// Перемещает текущий указатель инструкции на заданный.
  /// Например: JUMP "condition"
{JUMP, 0},

  /// Сравнивает два числа из операндового стека, кладёт результат в флаги EQ, NE, LT, LE, GT, GE
  /// Например: PUSH 2; PUSH 3; CMP;
{CMP, 2},

  /// Прыгает на заданную метку если флаг EQ = true
{JUMP_EQ, 0},

  /// Прыгает на заданную метку если флаг NE = true
{JUMP_NE, 0},

  /// Прыгает на заданную метку если флаг LT = true
{JUMP_LT, 0},

  /// Прыгает на заданную метку если флаг LE = true
{JUMP_LE, 0},

  /// Прыгает на заданную метку если флаг GT = true
{JUMP_GT, 0},

  /// Прыгает на заданную метку если флаг GE = true
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

  FunctionElimination(bytecode, mask);
  VariableElimination(bytecode, mask);
  FunctionElimination(bytecode, mask);

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
