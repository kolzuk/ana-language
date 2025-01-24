#ifndef JIT_JITEXECUTOR_H_
#define JIT_JITEXECUTOR_H_

#include "GarbageCollector.h"

#include <asmjit/asmjit.h>

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
  Logger* Logger;
  x86::Assembler* CurrentAssembler;
  GarbageCollector GC;

  std::unordered_map<std::string, NativeFunction> FuncMap;
  const char* CurFunctionName{};
  Label CurFunStartLabel;
  FuncSignature CurFunctionSignature;

  std::unordered_map<std::string, Label> Labels;
  std::unordered_map<std::string, int> VarIdx;
  int CurIdx = 0;

 public:
  Label getLabel(const char* LabelName);
  void newVar(const char* Name);
  void newVar(const char* Name, const x86::Gp& Value);
  x86::Mem getVarMem(const char* Name);
  x86::Mem getVarMemByIdx(const char* Name, int64_t Idx);
  void assignVar(const char* Name, const x86::Gp& Src);
  void getValue(const char* Name, const x86::Gp& Dst);
  void getValueByIdx(const char* Name, int64_t Idx, const x86::Gp& Dst);

  void push(int64_t x);
  void assign(std::string Name);
  void assignByIndex(std::string Name);
  void assignByIndex(std::string Name);
  void allocNewArray(int64_t Size);
  void initGC();
  void callGC();

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
};

#endif JIT_JITEXECUTOR_H_
