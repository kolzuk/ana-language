#ifndef JIT_JIT_INTERPRETER_H_
#define JIT_JIT_INTERPRETER_H_

#include "GarbageCollector.h"

#include <asmjit/asmjit.h>
#include <string>
#include <stdexcept>
#include <unordered_map>

using namespace asmjit;

typedef void (* Func)();

struct NativeFunction {
  Func Ptr;
  FuncSignature Signature;
};

class Interpreter {
  const char* BufferStart = nullptr;
  const char* BufferPtr = nullptr;
  bool HasError = false;

  JitRuntime Runtime;
  CodeHolder* CurrentCode;
  Logger* Logger;
  x86::Assembler* CurrentAssembler;

  std::unordered_map<std::string, NativeFunction> FuncMap;
  const char* CurFunctionName{};
  Label CurFunStartLabel;
  FuncSignature CurFunctionSignature;

  std::unordered_map<std::string, Label> Labels;
  std::unordered_map<std::string, int> VarIdx;
  int CurIdx = 0;

  void error() {
    HasError = true;
    printf("Unexpected %c, Index %lld\n", *BufferPtr, BufferPtr - BufferStart);
  }

  void error(Error error) {
    HasError = true;
    printf("Error: %s\n", DebugUtils::errorAsString(error));
  }

  void parse();
  void parseFunction();
  void parseStatement();
  void parseAdd();
  void parseSUB();
  void parseMUL();
  void parseDIV();
  void parseMOD();
  void parseAssign();
  void parseNewInt();
  void parseNewArray();
  void parsePrint();
  void parseReturn();
  void parseReturnVoid();
  void parseCall();
  void parseGOTO();
  void parseBlock();
  void parseEQ();
  void parseNE();
  void parseLT();
  void parseLE();
  void parseGT();
  void parseGE();

  void parseOperand(const x86::Gp& Dst);
  x86::Mem parseAssignableOperand();
  const char* parseName();
  int64_t parseInt64();

  Label getLabel(const char* LabelName) {
    if (Labels.find(LabelName) == Labels.end()) {
      Labels[LabelName] = CurrentAssembler->newLabel();
    }
    return Labels[LabelName];
  }

  void newVar(const char* Name) {
    VarIdx[Name] = CurIdx;
    CurrentAssembler->push(0);
    ++CurIdx;
  }

  void newVar(const char* Name, const x86::Gp& Value) {
    VarIdx[Name] = CurIdx;
    CurrentAssembler->push(Value);
    ++CurIdx;
  }

  x86::Mem getVarMem(const char* Name) {
    return x86::ptr(x86::rsp, (CurIdx - VarIdx[Name] - 1) * 8);
  }

  x86::Mem getVarMemByIdx(const char* Name, int64_t Idx) {
    CurrentAssembler->mov(x86::r15, getVarMem(Name));
    return x86::ptr(x86::r15, Idx * 8);
  }

  void assignVar(const char* Name, const x86::Gp& Src) {
    CurrentAssembler->mov(getVarMem(Name), Src);
  }

  void getValue(const char* Name, const x86::Gp& Dst) {
    CurrentAssembler->mov(Dst, getVarMem(Name));
  }

  void getValueByIdx(const char* Name, int64_t Idx, const x86::Gp& Dst) {
    CurrentAssembler->mov(x86::r15, getVarMem(Name));
    CurrentAssembler->mov(Dst, x86::ptr(x86::r15, Idx * 8));
  }

  void allocNewArray(int64_t Size, const x86::Gp& Dst) {
    auto* Arr = new int64_t[Size];
    CurrentAssembler->mov(Dst, Arr);
  }

  void resetCompiler() {
    Labels.clear();
    VarIdx.clear();
    CurIdx = 0;
    delete CurrentCode;

    CurrentCode = new CodeHolder();
    CurrentCode->init(Runtime.environment(), Runtime.cpuFeatures());
    CurrentCode->setLogger(Logger);
    CurrentAssembler = new x86::Assembler(CurrentCode);
  }

 public:
  Interpreter() {
    CurrentCode = new CodeHolder();
    CurrentCode->init(Runtime.environment(), Runtime.cpuFeatures());
    Logger = new FileLogger(stdout);
    CurrentCode->setLogger(Logger);
    CurrentAssembler = new x86::Assembler(CurrentCode);
  }

  void execute(const char*);
  ~Interpreter() {
    for (const auto& FuncElem : FuncMap) {
      Runtime.release(FuncElem.second.Ptr);
    }

    delete CurrentAssembler;
    delete CurrentCode;
    delete Logger;
  }

  bool hasError() const {
    return HasError;
  }
};

enum OpCode : char {
  ADD = 1,
  SUB,
  MUL,
  DIV,
  MOD,
  ASSIGN,
  FUN,
  NEW_INT,
  NEW_ARRAY,
  PRINT,
  RETURN,
  RETURN_VOID,
  GOTO,
  CMP_EQ,
  CMP_NE,
  CMP_LT,
  CMP_LE,
  CMP_GT,
  CMP_GE,
  BLOCK,
  CALL,
  FUN_END,
  END
};

enum BytecodeType : char {
  INT_TYPE = 1,
  PTR_TYPE = 2,
  VOID_TYPE = 3,
  INT_LITERAL = 4,
  INDEX = 5,
};

#endif //JIT_JIT_INTERPRETER_H_
