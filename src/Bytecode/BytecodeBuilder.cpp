#include "Bytecode/BytecodeBuilder.h"

std::vector<std::pair<Operation, std::vector<std::string>>> BytecodeBuilder::build() {
  return Bytecode;
}

void BytecodeBuilder::add() {
  Bytecode.push_back({Operation::ADD, {}});
}

void BytecodeBuilder::sub() {
  Bytecode.push_back({Operation::SUB, {}});
}

void BytecodeBuilder::mul() {
  Bytecode.push_back({Operation::MUL, {}});
}

void BytecodeBuilder::div() {
  Bytecode.push_back({Operation::DIV, {}});
}

void BytecodeBuilder::mod() {
  Bytecode.push_back({Operation::MOD, {}});
}

void BytecodeBuilder::push(const std::string& x) {
  Bytecode.push_back({Operation::PUSH, {x}});
}

void BytecodeBuilder::integerLoad(const std::string& Name) {
  Bytecode.push_back({Operation::INTEGER_LOAD, {Name}});
}

void BytecodeBuilder::arrayLoad(const std::string& Name) {
  Bytecode.push_back({Operation::ARRAY_LOAD, {Name}});
}

void BytecodeBuilder::loadFromIndex(const std::string& Name) {
  Bytecode.push_back({Operation::LOAD_FROM_INDEX, {Name}});
}

void BytecodeBuilder::integerStore(const std::string& Name) {
  Bytecode.push_back({Operation::INTEGER_STORE, {Name}});
}

void BytecodeBuilder::arrayStore(const std::string& Name) {
  Bytecode.push_back({Operation::ARRAY_STORE, {Name}});
}

void BytecodeBuilder::storeInIndex(const std::string& Name) {
  Bytecode.push_back({Operation::STORE_IN_INDEX, {Name}});
}

void BytecodeBuilder::newArray() {
  Bytecode.push_back({Operation::NEW_ARRAY, {}});
}

void BytecodeBuilder::print() {
  Bytecode.push_back({Operation::PRINT, {}});
}

void BytecodeBuilder::funBegin(const std::vector<std::string>& Names) {
  Bytecode.emplace_back(Operation::FUN_BEGIN, Names);
}

void BytecodeBuilder::funEnd() {
  Bytecode.push_back({Operation::FUN_END, {}});
}

void BytecodeBuilder::funCall(const std::string& Name) {
  Bytecode.push_back({Operation::FUN_CALL, {Name}});
}

void BytecodeBuilder::addReturn() {
  Bytecode.push_back({Operation::RETURN, {}});
}

void BytecodeBuilder::jump(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP, {Label}});
}

void BytecodeBuilder::label(const std::string& Label) {
  Bytecode.push_back({Operation::LABEL, {Label}});
}

void BytecodeBuilder::cmp() {
  Bytecode.push_back({Operation::CMP, {}});
}

void BytecodeBuilder::jumpEq(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP_EQ, {Label}});
}

void BytecodeBuilder::jumpNe(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP_NE, {Label}});
}

void BytecodeBuilder::jumpLt(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP_LT, {Label}});
}

void BytecodeBuilder::jumpLe(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP_LE, {Label}});
}

void BytecodeBuilder::jumpGt(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP_GT, {Label}});
}

void BytecodeBuilder::jumpGe(const std::string& Label) {
  Bytecode.push_back({Operation::JUMP_GE, {Label}});
}

std::pair<Operation, std::vector<std::string>> BytecodeBuilder::getLastCommand() {
  return Bytecode.back();
}
