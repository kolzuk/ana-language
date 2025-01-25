#ifndef JIT_JITEXECUTOR_H_
#define JIT_JITEXECUTOR_H_

#include "GarbageCollector.h"

#include <asmjit/asmjit.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <unordered_map>

using namespace asmjit;

typedef void (* Func)();

struct NativeFunction {
  Func Ptr;
  FuncSignature Signature;
};

class JitExecutor {
  JitRuntime Runtime;
  CodeHolder* CurrentCode;
//  Logger* Logger;
  x86::Assembler* CurrentAssembler;
  GarbageCollector GC;

  std::unordered_map<std::string, NativeFunction> FuncMap;
  std::string CurFunctionName{};
  Label CurFunStartLabel;
  FuncSignature CurFunctionSignature;

  std::unordered_map<std::string, Label> Labels;
  std::unordered_map<std::string, int> VarIdx;
  int CurIdx = 0;

  Label getLabel(const std::string& LabelName);
  void newVar(const std::string& Name);
  void newVar(const std::string& Name, const x86::Gp& Value);
  x86::Mem getVarMem(const std::string& Name);
  x86::Mem getVarMemByIdx(const std::string& Name, const x86::Gp& Idx);
  void assignVar(const std::string& Name, const x86::Gp& Src);
  void assignVarByIdx(const std::string& Name, const x86::Gp& Idx, const x86::Gp& Src);
  void getValue(const std::string& Name, const x86::Gp& Dst);
  void getValueByIdx(const std::string& Name, const x86::Gp& Idx, const x86::Gp& Dst);

  static void error(Error error) {
    printf("Error: %s\n", DebugUtils::errorAsString(error));
  }

  void pushStack(const x86::Gp& Src) {
    ++CurIdx;
    CurrentAssembler->push(Src);
  }

  void pushStack(int64_t x) {
    ++CurIdx;
    CurrentAssembler->push(x);
  }

  void popStack(const x86::Gp& Dst) {
    --CurIdx;
    CurrentAssembler->pop(Dst);
  }

 public:
  JitExecutor() {
    CurrentCode = new CodeHolder();
    CurrentCode->init(Runtime.environment(), Runtime.cpuFeatures());
//    Logger = new FileLogger(llvm::raw_ostream);
//    CurrentCode->setLogger(Logger);
    CurrentAssembler = new x86::Assembler(CurrentCode);
  }

  void resetCompiler() {
    Labels.clear();
    VarIdx.clear();
    CurIdx = 0;
    delete CurrentAssembler;
    delete CurrentCode;

    CurrentCode = new CodeHolder();
    CurrentCode->init(Runtime.environment(), Runtime.cpuFeatures());
//    CurrentCode->setLogger(Logger);
    CurrentAssembler = new x86::Assembler(CurrentCode);
  }

  ~JitExecutor() {
    for (const auto& FuncElem : FuncMap) {
      Runtime.release(FuncElem.second.Ptr);
    }
    delete CurrentAssembler;
    delete CurrentCode;
//    delete Logger;
  }

  Func lookupMain();
  void addFunction(std::vector<std::string> Names);
  void endFunction();
  void addFunctionCall(const std::string& Name);
  void addReturn();
  void print();

  void add();
  void sub();
  void mul();
  void div();
  void mod();

  void push(int64_t x);
  void assign(const std::string& Name);
  void assignByIndex(const std::string& Name);
  void load(const std::string& Name);
  void loadByIndex(const std::string& Name);

  void allocNewArray(const std::string& Name);
  void initGC();
  void callGC();

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

#endif JIT_JITEXECUTOR_H_
