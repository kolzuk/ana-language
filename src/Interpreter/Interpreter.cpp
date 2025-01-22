#include "Interpreter/Interpreter.h"
#include <vector>

enum OpCode : char {
  ADD,
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
  INT_TYPE,
  PTR_TYPE,
  VOID_TYPE,
  INT_LITERAL,
  INDEX,
};

std::string Interpreter::parseName() {
  auto BufferEnd = BufferPtr + 1;
  while (*BufferEnd != 0) {
    ++BufferEnd;
  }
  std::string Name(BufferPtr, BufferEnd - BufferPtr);
  printf("parsed name: %s\n", Name.c_str());
  BufferPtr = BufferEnd + 1;
  return Name;
}

int64_t Interpreter::parseInt64() {
  int64_t Res = 0;
  for (int i = 0; i < sizeof(Res); i++) {
    Res = (Res << 8) + *BufferPtr; // big endian?
    ++BufferPtr;
  }
  return Res;
}

void Interpreter::parseFunction() {
  resetCompiler();
  ++BufferPtr;

  auto FunName = parseName();
  FuncSignature Signature(CallConvId::kHost);

  CurFunctionName = FunName;
  CurFunctionSignature = Signature;

  switch (*BufferPtr) {
    case (BytecodeType::INT_TYPE) : Signature.setRet(TypeId::kInt64);
      break;
    case (BytecodeType::PTR_TYPE) : Signature.setRet(TypeId::kIntPtr);
      break;
    case (BytecodeType::VOID_TYPE) : Signature.setRet(TypeId::kVoid);
      break;
    default:error();
  }
  ++BufferPtr;

  std::vector<x86::Gp> ArgRegs;
  std::vector<std::string> ArgNames;
  std::string ArgName;
  x86::Gp ArgReg;
  while (*BufferPtr != 0) {
    switch (*BufferPtr) {
      case (BytecodeType::INT_TYPE) :ArgReg = CurrentCompiler->newInt64();
        Signature.addArg(TypeId::kInt64);
        ++BufferPtr;
        ArgName = parseName();
        newInt(ArgName);
        break;
      case (BytecodeType::PTR_TYPE) :ArgReg = CurrentCompiler->newIntPtr();
        Signature.addArg(TypeId::kIntPtr);
        ++BufferPtr;
        ArgName = parseName();
        newPtr(ArgName);
        break;
      default:error();
    }
    ArgRegs.push_back(ArgReg);
    ArgNames.push_back(ArgName);
  }
  ++BufferPtr;

  FuncNode* FuncNode = CurrentCompiler->addFunc(Signature);
  CurFunctionNode = FuncNode;
  for (int i = 0; i < ArgRegs.size(); i++) {
    FuncNode->setArg(i, ArgRegs[i]);
  }

  for (int i = 0; i < ArgRegs.size(); i++) {
    getVariablePtr(ArgNames[i], ArgRegs[i]);
  }

  while (*BufferPtr != OpCode::FUN_END) {
    parseStatement();
  }
  ++BufferPtr;

  CurrentCompiler->endFunc();
  Func Func;
  Error err = Runtime.add(&Func, CurrentCode);

  if (err) {
    error(err);
    return;
  }

  FuncMap[FunName] = NativeFunction{Func, Signature};
  CurrentCompiler->finalize();
}

void Interpreter::parseStatement() {
  switch (*BufferPtr) {
    case ADD:parseAdd();
      break;
    case SUB:parseSUB();
      break;
    case MUL:parseMUL();
      break;
    case DIV:parseDIV();
      break;
    case MOD:parseMOD();
      break;
    case ASSIGN:parseAssign();
      break;
    case NEW_INT:parseNewInt();
      break;
    case NEW_ARRAY:parseNewArray();
      break;
    case PRINT:parsePrint();
      break;
    case RETURN:parseReturn();
      break;
    case RETURN_VOID:parseReturnVoid();
      break;
    case CALL:parseCall();
      break;
    case GOTO:parseGOTO();
      break;
    case BLOCK:parseBlock();
      break;
    case CMP_EQ:parseEQ();
      break;
    case CMP_NE:parseNE();
      break;
    case CMP_LT:parseLT();
      break;
    case CMP_LE:parseLE();
      break;
    case CMP_GT:parseGT();
      break;
    case CMP_GE:parseGE();
      break;
    default:error();
  }
}

void Interpreter::parse() {
  if (*BufferPtr == OpCode::FUN) {
    parseFunction();
  } else {
    error();
  }
}

void Interpreter::execute(const char* Bytecode) {
  BufferStart = Bytecode;
  BufferPtr = Bytecode;
  HasError = false;
  while (*BufferPtr != OpCode::END) {
    parse();
  }
}

void Interpreter::parseAssignableOperand(const x86::Gp& Dst) {
  if (*BufferPtr == BytecodeType::INT_TYPE || *BufferPtr == BytecodeType::PTR_TYPE) {
    ++BufferPtr;
    auto Name = parseName();
    getVariablePtr(Name, Dst);
  } else if (*BufferPtr == BytecodeType::INDEX) {
    ++BufferPtr;
    auto Name = parseName();
    auto Ind = parseInt64();
    getVariablePtr(Name, Dst);
    CurrentCompiler->mov(Dst, x86::ptr(Dst));
    CurrentCompiler->add(Dst, Ind * 8);
  } else {
    error();
  }
}

void Interpreter::parseOperand(const x86::Gp& Dst) {
  if (*BufferPtr == BytecodeType::INT_TYPE || *BufferPtr == BytecodeType::PTR_TYPE) {
    ++BufferPtr;
    auto Name = parseName();
    getVariablePtr(Name, Dst);
    CurrentCompiler->mov(Dst, x86::ptr(Dst));
  } else if (*BufferPtr == BytecodeType::INT_LITERAL) {
    ++BufferPtr;
    auto Op = parseInt64();
    CurrentCompiler->mov(Dst, Op);
  } else if (*BufferPtr == BytecodeType::INDEX) {
    ++BufferPtr;
    auto Name = parseName();
    auto Ind = parseInt64();
    getVariablePtr(Name, Dst);
    CurrentCompiler->mov(Dst, x86::ptr(Dst));
    CurrentCompiler->mov(Dst, x86::ptr(Dst, Ind * 8));
  } else {
    error();
  }
}

void Interpreter::parseAdd() {
  ++BufferPtr;
  parseAssignableOperand(x86::rax);
  parseOperand(x86::rbx);
  CurrentCompiler->add(x86::ptr(x86::rax), x86::rbx);
}

void Interpreter::parseSUB() {
  ++BufferPtr;
  parseAssignableOperand(x86::rax);
  parseOperand(x86::rbx);
  CurrentCompiler->sub(x86::ptr(x86::rax), x86::rbx);
}

void Interpreter::parseMUL() {
  ++BufferPtr;
  parseAssignableOperand(x86::rcx);
  parseOperand(x86::rbx);

  CurrentCompiler->mov(x86::rax, x86::ptr(x86::rcx));
  CurrentCompiler->xor_(x86::rdx, x86::rdx);

  CurrentCompiler->xor_(x86::r8, x86::r8);
  CurrentCompiler->xor_(x86::r9, x86::r9);

  CurrentCompiler->imul(x86::r8, x86::r9, x86::rbx);
  CurrentCompiler->mov(x86::ptr(x86::rcx), x86::rax);
}

void Interpreter::parseDIV() {
  ++BufferPtr;
  parseAssignableOperand(x86::rcx);
  parseOperand(x86::rbx);

  CurrentCompiler->mov(x86::rax, x86::ptr(x86::rcx));
  CurrentCompiler->cqo(x86::rdx, x86::rax);

  CurrentCompiler->xor_(x86::r8, x86::r8);
  CurrentCompiler->xor_(x86::r9, x86::r9);

  CurrentCompiler->idiv(x86::r8, x86::r9, x86::rbx);
  CurrentCompiler->mov(x86::ptr(x86::rcx), x86::rax);
}

void Interpreter::parseMOD() {
  ++BufferPtr;
  parseAssignableOperand(x86::rcx);
  parseOperand(x86::rbx);

  CurrentCompiler->mov(x86::rax, x86::ptr(x86::rcx));
  CurrentCompiler->cqo(x86::rdx, x86::rax);

  CurrentCompiler->xor_(x86::r8, x86::r8);
  CurrentCompiler->xor_(x86::r9, x86::r9);

  CurrentCompiler->idiv(x86::r8, x86::r9, x86::rbx);
  CurrentCompiler->mov(x86::ptr(x86::rcx), x86::rdx);
}

void Interpreter::parseAssign() {
  ++BufferPtr;
  parseAssignableOperand(x86::rax);
  parseOperand(x86::rbx);
  CurrentCompiler->mov(x86::ptr(x86::rax), x86::rbx);
}

void Interpreter::parseNewInt() {
  ++BufferPtr;
  auto Name = parseName();
  newInt(Name);
}

void Interpreter::parseNewArray() {
  ++BufferPtr;
  auto Name = parseName();
  auto Size = parseInt64();
  newArrayAlloc(Name, Size);
}

void print(int64_t x) {
  printf("%lld\n", x);
}

void Interpreter::parsePrint() {
  ++BufferPtr;
  auto PrintArg = CurrentCompiler->newInt64();
  parseOperand(PrintArg);

  void* PrintAddr = reinterpret_cast<void*>(&print);
  FuncSignature PrintSignature(CallConvId::kHost);
  PrintSignature.setRet(TypeId::kVoid);
  PrintSignature.addArg(TypeId::kInt64);

  InvokeNode* InvokeNode;
  CurrentCompiler->invoke(&InvokeNode, PrintAddr, PrintSignature);
  InvokeNode->setArg(0, PrintArg);
}

void Interpreter::parseReturn() {
  ++BufferPtr;
  parseOperand(x86::rax);
  CurrentCompiler->ret();
}

void Interpreter::parseReturnVoid() {
  ++BufferPtr;
  CurrentCompiler->ret();
}

void Interpreter::parseCall() {
  ++BufferPtr;
  auto FuncName = parseName();
  FuncSignature Signature;

  if (FuncName == CurFunctionName) {
    Signature = CurFunctionSignature;
  } else {
    Signature = FuncMap[FuncName].Signature;
  }

  std::vector<x86::Gp> ArgRegs;
  for (int i = 0; i < Signature.argCount(); i++) {
    auto ArgReg = CurrentCompiler->newGp(Signature.args()[i]);
    parseOperand(ArgReg);
    ArgRegs.push_back(ArgReg);
  }

  newFunctionCall();
  InvokeNode* Invoke;
  if (FuncName == CurFunctionName) {
    CurrentCompiler->invoke(&Invoke, CurFunctionNode->label(), Signature);
  } else {
    CurrentCompiler->invoke(&Invoke, FuncMap[FuncName].Address, Signature);
  }
  functionRet();

  for (int i = 0; i < Signature.argCount(); i++) {
    Invoke->setArg(i, ArgRegs[i]);
  }
  if (Signature.ret() != TypeId::kVoid) {
    auto RetValue = CurrentCompiler->newGp(Signature.ret());
    Invoke->setRet(0, RetValue);
    auto RetPtr = CurrentCompiler->newIntPtr();
    parseAssignableOperand(RetPtr);
    CurrentCompiler->mov(x86::ptr(RetPtr), RetValue);
  }
}

Label Interpreter::getLabel(const std::string& LabelName) {
  if (Labels.find(LabelName) == Labels.end()) {
    Labels[LabelName] = CurrentCompiler->newLabel();
  }
  return Labels[LabelName];
}

void Interpreter::parseGOTO() {
  ++BufferPtr;
  auto Label = parseName();
  CurrentCompiler->jmp(getLabel(Label));
}

void Interpreter::parseBlock() {
  ++BufferPtr;
  auto Label = parseName();
  CurrentCompiler->bind(getLabel(Label));
}

void Interpreter::parseEQ() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentCompiler->cmp(x86::rax, x86::rbx);
  CurrentCompiler->je(getLabel(Label));
}

void Interpreter::parseNE() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentCompiler->cmp(x86::rax, x86::rbx);
  CurrentCompiler->jne(getLabel(Label));
}

void Interpreter::parseLT() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentCompiler->cmp(x86::rax, x86::rbx);
  CurrentCompiler->jl(getLabel(Label));
}

void Interpreter::parseLE() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentCompiler->cmp(x86::rax, x86::rbx);
  CurrentCompiler->jle(getLabel(Label));
}

void Interpreter::parseGT() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentCompiler->cmp(x86::rax, x86::rbx);
  CurrentCompiler->jg(getLabel(Label));
}

void Interpreter::parseGE() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentCompiler->cmp(x86::rax, x86::rbx);
  CurrentCompiler->jge(getLabel(Label));
}

int64_t* getVarPtrCall(GarbageCollector* GC, char* Name) {
  return GC->getVariablePtr(Name);
}

void Interpreter::getVariablePtr(const std::string& Name, const x86::Gp& Dst) {
  InvokeNode* Invoke;
  FuncSignature Signature = FuncSignature::build<int64_t*, void*, char*>();
  x86::Gp ArgReg1 = CurrentCompiler->newIntPtr();
  x86::Gp ArgReg2 = CurrentCompiler->newIntPtr();

  CurrentCompiler->mov(ArgReg1, &GC);
  CurrentCompiler->mov(ArgReg2, Name.c_str());

  CurrentCompiler->invoke(&Invoke, &getVarPtrCall, Signature);
  Invoke->setArg(0, ArgReg1);
  Invoke->setArg(1, ArgReg2);
  Invoke->setRet(0, Dst);
}

void newIntCall(GarbageCollector* GC, char* Name) {
  GC->newInt(Name);
}

void Interpreter::newInt(const std::string& Name) {
  InvokeNode* Invoke;
  FuncSignature Signature = FuncSignature::build<void, void*, char*>();
  x86::Gp ArgReg1 = CurrentCompiler->newIntPtr();
  x86::Gp ArgReg2 = CurrentCompiler->newIntPtr();

  CurrentCompiler->mov(ArgReg1, &GC);
  CurrentCompiler->mov(ArgReg2, Name.c_str());

  CurrentCompiler->invoke(&Invoke, &newIntCall, Signature);
  Invoke->setArg(0, ArgReg1);
  Invoke->setArg(1, ArgReg2);
}

void newPtrCall(GarbageCollector* GC, char* Name) {
  GC->newPtr(Name);
}

void Interpreter::newPtr(const std::string& Name) {
  InvokeNode* Invoke;
  FuncSignature Signature = FuncSignature::build<void, void*, char*>();
  x86::Gp ArgReg1 = CurrentCompiler->newIntPtr();
  x86::Gp ArgReg2 = CurrentCompiler->newIntPtr();

  CurrentCompiler->mov(ArgReg1, &GC);
  CurrentCompiler->mov(ArgReg2, Name.c_str());

  CurrentCompiler->invoke(&Invoke, &newPtrCall, Signature);
  Invoke->setArg(0, ArgReg1);
  Invoke->setArg(1, ArgReg2);
}

void newArrayAllocCall(GarbageCollector* GC, char* Name, int64_t Size) {
  GC->newArrayAlloc(Name, Size);
}

void Interpreter::newArrayAlloc(const std::string& Name, int64_t Size) {
  InvokeNode* Invoke;
  FuncSignature Signature = FuncSignature::build<void, void*, char*, int64_t>();
  x86::Gp ArgReg1 = CurrentCompiler->newIntPtr();
  x86::Gp ArgReg2 = CurrentCompiler->newIntPtr();
  x86::Gp ArgReg3 = CurrentCompiler->newIntPtr();

  CurrentCompiler->mov(ArgReg1, &GC);
  CurrentCompiler->mov(ArgReg2, Name.c_str());
  CurrentCompiler->mov(ArgReg3, Size);

  CurrentCompiler->invoke(&Invoke, &newArrayAllocCall, Signature);
  Invoke->setArg(0, ArgReg1);
  Invoke->setArg(1, ArgReg2);
  Invoke->setArg(2, ArgReg3);
}

void newScopeCall(GarbageCollector* GC) {
  GC->newScope();
}

void Interpreter::newFunctionCall() {
  InvokeNode* Invoke;
  FuncSignature Signature = FuncSignature::build<void, void*>();
  x86::Gp ArgReg1 = CurrentCompiler->newIntPtr();

  CurrentCompiler->mov(ArgReg1, &GC);

  CurrentCompiler->invoke(&Invoke, &newScopeCall, Signature);
  Invoke->setArg(0, ArgReg1);
}

void destroyScopeCall(GarbageCollector* GC) {
  GC->destroyScope();
}

void Interpreter::functionRet() {
  InvokeNode* Invoke;
  FuncSignature Signature = FuncSignature::build<void, void*>();
  x86::Gp ArgReg1 = CurrentCompiler->newIntPtr();

  CurrentCompiler->mov(ArgReg1, &GC);

  CurrentCompiler->invoke(&Invoke, &destroyScopeCall, Signature);
  Invoke->setArg(0, ArgReg1);
}
