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
  Func Address;
  FuncSignature Signature;
};

class Interpreter {
  const char* BufferStart = nullptr;
  const char* BufferPtr = nullptr;
  bool HasError = false;
  JitRuntime Runtime;
  std::unordered_map<std::string, NativeFunction> FuncMap;
  GarbageCollector GC;
  CodeHolder* CurrentCode;
  x86::Compiler* CurrentCompiler;
  std::unordered_map<std::string, Label> Labels;
  std::string CurFunctionName;
  FuncNode* CurFunctionNode;
  FuncSignature CurFunctionSignature;

  void error() {
    HasError = true;
    printf("Unexpected %c\n", *BufferPtr);
  }

  void error(Error error) {
    HasError = true;
    printf("Error: %s\n", DebugUtils::errorAsString(error));
  }

  void parse();
  void parseFunction();
  void parseStatement();
  void parseAdd();
  void parseSub();
  void parseMul();
  void parseDiv();
  void parseMod();
  void parseAssign();
  void parseNewInt();
  void parseNewArray();
  void parsePrint();
  void parseReturn();
  void parseReturnVoid();
  void parseCall();
  void parseGoto();
  void parseBlock();
  void parseEQ();
  void parseNE();
  void parseLT();
  void parseLE();
  void parseGT();
  void parseGE();

  void parseOperand(const x86::Gp& Dst);
  void parseAssignableOperand(const x86::Gp& Dst);
  std::string parseName();
  int64_t parseInt64();

  void getVariablePtr(const std::string& Name, const x86::Gp& Dst);
  void newInt(const std::string& Name);
  void newPtr(const std::string& Name);
  void newArrayAlloc(const std::string& Name, int64_t Size);
  void runtimeFunctionCall();
  void runtimeFunctionReturn();
  Label getLabel(const std::string& LabelName);

  void resetCompiler() {
    Labels.clear();
    delete CurrentCompiler;
    delete CurrentCode;

    CurrentCode = new CodeHolder();
    CurrentCode->init(Runtime.environment(), Runtime.cpuFeatures());
    FileLogger Logger(stdout);
    CurrentCode->setLogger(&Logger);
    CurrentCompiler = new x86::Compiler(CurrentCode);
  }

 public:
  Interpreter() {
    CurrentCode = new CodeHolder();
    CurrentCode->init(Runtime.environment(), Runtime.cpuFeatures());
    FileLogger Logger(stdout);
    CurrentCode->setLogger(&Logger);
    CurrentCompiler = new x86::Compiler(CurrentCode);
  }

  void execute(const char*);
  ~Interpreter() {
    for (const auto& FuncElem : FuncMap) {
      Runtime.release(FuncElem.second.Address);
    }

    delete CurrentCompiler;
    delete CurrentCode;
  }

  bool hasError() {
    return HasError;
  }
};

#endif //JIT_JIT_INTERPRETER_H_
