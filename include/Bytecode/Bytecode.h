#ifndef BYTECODE_H
#define BYTECODE_H

#include <string>

enum Operation : char {
  /// Берёт два значения из стека операндов, суммирует их,
  /// и кладёт полученное значение обратно в операндовый стек.
  ADD = 0,

  /// Берёт два значения из стека операндов, вычитает одно значение из другого,
  /// и кладёт полученное значение обратно в операндовый стек.
  SUB = 1,

  /// Берёт два значения из стека операндов, умножает их,
  /// и кладёт полученное значение обратно в операндовый стек.
  MUL = 2,

  /// Берёт два значения из стека операндов, делит одно значение на другое,
  /// и кладёт полученное значение обратно в операндовый стек.
  DIV = 3,

  /// Берёт два значения из стека операндов, берёт одно значение по модулю второго,
  /// и кладёт полученное значение обратно в операндовый стек.
  MOD = 4,

  /// Кладёт значение в операндовый стек (только 8-и байтное значение)
  /// Например: PUSH "8"
  PUSH = 5,

  /// Загружает значение переменной в операндовый стек (только 8-и байтное значение)
  /// Например: INTEGER_LOAD "a"
  INTEGER_LOAD = 6,

  /// Загружает указатель массива в операндовый стек (только 8-и байтное значение)
  /// Например: ARRAY_LOAD "array"
  ARRAY_LOAD = 7,

  /// Загружает значение массива в позиции i в операндовый стек (только 8-и байтное значение)
  /// Например: LOAD_FROM_INDEX "array" 2
  /// Например: LOAD_FROM_INDEX 12, 3
  LOAD_FROM_INDEX = 8,

  /// Загружает в значение переменной значение из операндового стека (только 8-и байтное значение).
  /// Например: INTEGER_STORE "a"
  INTEGER_STORE = 9,

  /// Загружает в указатель массива значение из операндового стека
  ARRAY_STORE = 10,

  /// Загружает в значение элемента массива по индексу
  /// значение из операндового стека (только 8-и байтное значение).
  /// Индекс берёт из стека.
  /// Например: STORE_IN_INDEX "a"
  STORE_IN_INDEX = 11,

  /// Инициализирует массив. Предварительно нужно положить размер массива
  /// в операндовый стек. Затем кладёт указатель на вершину стека.
  /// Например: PUSH 10; NEW ARRAY; // создаёт массив размером 10.
  NEW_ARRAY = 12,

  /// Выводит в std::cout последнее значение из операндового стека.
  /// Например: PUSH 1; PRINT; // std::cout << 10; значение из операндового стека убирается.
  PRINT = 13,

  /// Декларирует функцию следующим.
  /// Например: FUN_BEGIN foo integer a, integer b, array arr
  FUN_BEGIN = 14,

  /// Пометка о том, что функция закончилась.
  FUN_END = 15,

  /// Вызывает указанную функцию, параметры передаются через операндовый стек.
  /// Например: PUSH 1; INTEGER_LOAD a; FUN_CALL SUM;
  /// Код выше эквивалентен соответствующему: SUM(a, 1);
  FUN_CALL = 16,

  /// Возвращает верхнее значение из операндового стека в операндовый стек вызываемого стекфрейма.
  /// Сдвигает текущий указатель исполнения кода на вызвавший его стекфрейма.
  /// Например: PUSH 1; RETURN;
  RETURN = 17,

  /// Создаёт LABEL
  /// Например: LABEL "condition"
  LABEL = 18,

  /// Перемещает текущий указатель инструкции на заданный.
  /// Например: JUMP "condition"
  JUMP = 19,

  /// Сравнивает два числа из операндового стека, кладёт результат в флаги EQ, NE, LT, LE, GT, GE
  /// Например: PUSH 2; PUSH 3; CMP;
  CMP = 20,

  /// Прыгает на заданную метку если флаг EQ = true
  JUMP_EQ = 21,

  /// Прыгает на заданную метку если флаг NE = true
  JUMP_NE = 22,

  /// Прыгает на заданную метку если флаг LT = true
  JUMP_LT = 23,

  /// Прыгает на заданную метку если флаг LE = true
  JUMP_LE = 24,

  /// Прыгает на заданную метку если флаг GT = true
  JUMP_GT = 25,

  /// Прыгает на заданную метку если флаг GE = true
  JUMP_GE = 26
};

std::string ConvertOperationToString(Operation operation);

#endif //BYTECODE_H
