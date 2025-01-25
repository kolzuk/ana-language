#ifndef BYTECODEBUILDER_H_
#define BYTECODEBUILDER_H_

#include "Bytecode.h"

#include <string>
#include <vector>

class BytecodeBuilder {
  std::vector<std::pair<Operation, std::vector<std::string>>> Bytecode;

 public:
  std::vector<std::pair<Operation, std::vector<std::string>>> build();
  void addFunction(const std::vector<std::string>& Names);
  void endFunction();
  void addFunctionCall(const std::string& Name);
  void addReturn();
  void print();

  void add();
  void sub();
  void mul();
  void div();
  void mod();

  void push(const std::string& x);
  void assign(const std::string& Name);
  void storeArray(const std::string& Name);
  void assignByIndex(const std::string& Name);
  void load(const std::string& Name);
  void loadByIndex(const std::string& Name);
  void loadArray(const std::string& Name);
  void allocNewArray(const std::string& Name);

  void addGoto(const std::string& Label);
  void label(const std::string& Label);
  void cmp();
  void jumpEq(const std::string& Label);
  void jumpNe(const std::string& Label);
  void jumpLt(const std::string& Label);
  void jumpLe(const std::string& Label);
  void jumpGt(const std::string& Label);
  void jumpGe(const std::string& Label);
};

#endif //BYTECODEBUILDER_H_
