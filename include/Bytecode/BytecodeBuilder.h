#ifndef BYTECODEBUILDER_H_
#define BYTECODEBUILDER_H_

#include "Bytecode.h"

#include <string>
#include <vector>

class BytecodeBuilder {
  std::vector<std::pair<Operation, std::vector<std::string>>> Bytecode;

 public:
  std::vector<std::pair<Operation, std::vector<std::string>>> build();

  // Builder methods
  void add();
  void sub();
  void mul();
  void div();
  void mod();
  void push(const std::string& x);

  void integerLoad(const std::string& Name);
  void arrayLoad(const std::string& Name);
  void loadFromIndex(const std::string& Name);
  void integerStore(const std::string& Name);
  void arrayStore(const std::string& Name);
  void storeInIndex(const std::string& Name);
  void newArray();

  void print();
  void funBegin(const std::vector<std::string>& Names);
  void funEnd();
  void funCall(const std::string& Name);
  void addReturn();

  void label(const std::string& Label);
  void jump(const std::string& Label);
  void cmp();
  void jumpEq(const std::string& Label);
  void jumpNe(const std::string& Label);
  void jumpLt(const std::string& Label);
  void jumpLe(const std::string& Label);
  void jumpGt(const std::string& Label);
  void jumpGe(const std::string& Label);

  std::pair<Operation, std::vector<std::string>> getLastCommand();
};

#endif //BYTECODEBUILDER_H_