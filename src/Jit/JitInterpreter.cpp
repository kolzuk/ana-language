#include "Jit/JitInterpreter.h"

#ifdef _WIN64
const std::vector<x86::Gp> ArgRegs { x86::rcx, x86::rdx, x86::r8, x86::r9 };
#else
const std::vector<x86::Gp> ArgRegs { x86::rdi, x86::rsi, x86::rdx, x86::rcx, x86::r8, x86::r9 };
#endif

const char* JitInterpreter::parseName() {
  const char* Start = BufferPtr;
  while (*BufferPtr != 0) {
    ++BufferPtr;
  }
  ++BufferPtr;
  return Start;
}

int64_t JitInterpreter::parseInt64() {
  int64_t Res = 0;
  for (int i = 0; i < sizeof(Res); i++) {
    Res = (Res << 8) + *BufferPtr;
    ++BufferPtr;
  }
  return Res;
}

void JitInterpreter::parseFunction() {
  resetCompiler();
  ++BufferPtr;

  auto FuncName = parseName();
  FuncSignature Signature(CallConvId::kHost);

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

  std::vector<const char*> ArgNames;
  const char* ArgName;
  while (*BufferPtr != 0) {
    if (*BufferPtr == BytecodeType::INT_TYPE) {
      ++BufferPtr;
      ArgName = parseName();
      Signature.addArg(TypeId::kInt64);
    } else if (*BufferPtr == BytecodeType::PTR_TYPE) {
      ++BufferPtr;
      ArgName = parseName();
      Signature.addArg(TypeId::kIntPtr);
    } else {
      error();
      return;
    }
    ArgNames.push_back(ArgName);
  }
  ++BufferPtr;

  CurFunctionName = FuncName;
  CurFunctionSignature = Signature;
  if (strcmp(FuncName, "main") == 0) {
    initGC();
  }

  Label Start = CurrentAssembler->newLabel();
  CurrentAssembler->bind(Start);
  CurFunStartLabel = Start;
  for (int i = 0; i < ArgNames.size(); i++) {
    if (i < ArgRegs.size()) {
      newVar(ArgNames[i], ArgRegs[i]);
    } else {
      newVar(ArgName);
      CurrentAssembler->mov(x86::rax, x86::ptr(x86::rsp, (CurIdx + i + 1) * 8));
      assignVar(ArgNames[i], x86::rax);
    }
  }

  while (*BufferPtr != OpCode::FUN_END && !HasError) {
    parseStatement();
  }

  if (HasError) {
    return;
  }

  ++BufferPtr;

  Error err = CurrentAssembler->finalize();
  if (err) {
    error(err);
    return;
  }

  Func Function;
  err = Runtime.add(&Function, CurrentCode);

  if (err) {
    error(err);
    return;
  }

  FuncMap[FuncName] = NativeFunction{Function, Signature};
}

void JitInterpreter::parseStatement() {
  switch (*BufferPtr) {
    case ADD:parseAdd();
      break;
    case SUB:parseSub();
      break;
    case MUL:parseMul();
      break;
    case DIV:parseDiv();
      break;
    case MOD:parseMod();
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
    case GOTO:parseGoto();
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

void JitInterpreter::parse() {
  if (*BufferPtr == OpCode::FUN) {
    parseFunction();
  } else {
    error();
  }
}

void JitInterpreter::execute(const char* Bytecode) {
  BufferStart = Bytecode;
  BufferPtr = Bytecode;
  HasError = false;
  while (*BufferPtr != OpCode::END && !HasError) {
    parse();
  }
  if (HasError) {
    return;
  }

  if (FuncMap.find("main") != FuncMap.end()) {
    FuncMap["main"].Ptr();
  }
}

x86::Mem JitInterpreter::parseAssignableOperand() {
  if (*BufferPtr == BytecodeType::INT_TYPE || *BufferPtr == BytecodeType::PTR_TYPE) {
    ++BufferPtr;
    auto Name = parseName();
    return getVarMem(Name);
  } else if (*BufferPtr == BytecodeType::INDEX) {
    ++BufferPtr;
    auto Name = parseName();
    auto Idx = parseInt64();
    return getVarMemByIdx(Name, Idx);
  } else {
    error();
  }
}

void JitInterpreter::parseOperand(const x86::Gp& Dst) {
  if (*BufferPtr == BytecodeType::INT_TYPE || *BufferPtr == BytecodeType::PTR_TYPE) {
    ++BufferPtr;
    auto Name = parseName();
    getValue(Name, Dst);
  } else if (*BufferPtr == BytecodeType::INT_LITERAL) {
    ++BufferPtr;
    auto Op = parseInt64();
    CurrentAssembler->mov(Dst, Op);
  } else if (*BufferPtr == BytecodeType::INDEX) {
    ++BufferPtr;
    auto Name = parseName();
    auto Ind = parseInt64();
    getValueByIdx(Name, Ind, Dst);
  } else {
    error();
  }
}

void JitInterpreter::parseAdd() {
  ++BufferPtr;
  auto Mem = parseAssignableOperand();
  parseOperand(x86::rax);
  CurrentAssembler->add(Mem, x86::rax);
}

void JitInterpreter::parseSub() {
  ++BufferPtr;
  auto Mem = parseAssignableOperand();
  parseOperand(x86::rax);
  CurrentAssembler->sub(Mem, x86::rax);
}

void JitInterpreter::parseMul() {
  ++BufferPtr;
  auto Mem = parseAssignableOperand();
  CurrentAssembler->mov(x86::rax, Mem);
  CurrentAssembler->cqo(x86::rdx, x86::rax);

  parseOperand(x86::rbx);
  CurrentAssembler->xor_(x86::r8, x86::r8);
  CurrentAssembler->xor_(x86::r9, x86::r9);

  CurrentAssembler->imul(x86::r8, x86::r9, x86::rbx);
  CurrentAssembler->mov(Mem, x86::rax);
}

void JitInterpreter::parseDiv() {
  ++BufferPtr;
  auto Mem = parseAssignableOperand();
  CurrentAssembler->mov(x86::rax, Mem);
  CurrentAssembler->cqo(x86::rdx, x86::rax);

  parseOperand(x86::rbx);
  CurrentAssembler->cqo(x86::r8, x86::r8);
  CurrentAssembler->cqo(x86::r9, x86::r9);

  CurrentAssembler->idiv(x86::r8, x86::r9, x86::rbx);
  CurrentAssembler->mov(Mem, x86::rax);
}

void JitInterpreter::parseMod() {
  ++BufferPtr;
  auto Mem = parseAssignableOperand();
  CurrentAssembler->mov(x86::rax, Mem);
  CurrentAssembler->cqo(x86::rdx, x86::rax);

  parseOperand(x86::rbx);
  CurrentAssembler->cqo(x86::r8, x86::r8);
  CurrentAssembler->cqo(x86::r9, x86::r9);

  CurrentAssembler->idiv(x86::r8, x86::r9, x86::rbx);
  CurrentAssembler->mov(Mem, x86::rdx);
}

void JitInterpreter::parseAssign() {
  ++BufferPtr;
  auto Mem = parseAssignableOperand();
  parseOperand(x86::rax);
  CurrentAssembler->mov(Mem, x86::rax);
}

void JitInterpreter::parseNewInt() {
  ++BufferPtr;
  auto Name = parseName();
  newVar(Name);
}

void JitInterpreter::parseNewArray() {
  ++BufferPtr;
  auto Name = parseName();
  auto Size = parseInt64();
  newVar(Name);
  allocNewArray(Size);
  assignVar(Name, x86::rax);
}

void print(int64_t x) {
  printf("%lld\n", x);
}

void JitInterpreter::parsePrint() {
  ++BufferPtr;
  parseOperand(ArgRegs[0]);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(print);
  CurrentAssembler->add(x86::rsp, 40);
}

void JitInterpreter::parseReturn() {
  ++BufferPtr;
  parseOperand(x86::rax);
  CurrentAssembler->add(x86::rsp, CurIdx * 8);
  CurrentAssembler->push(x86::rax);
  callGC();
  CurrentAssembler->pop(x86::rax);
  CurrentAssembler->ret();
}

void JitInterpreter::parseReturnVoid() {
  ++BufferPtr;
  CurrentAssembler->add(x86::rsp, CurIdx * 8);
  callGC();
  CurrentAssembler->ret();
}

void JitInterpreter::parseCall() {
  ++BufferPtr;
  auto FuncName = parseName();
  FuncSignature Signature;

  if (strcmp(FuncName, CurFunctionName) == 0) {
    Signature = CurFunctionSignature;
  } else {
    Signature = FuncMap[FuncName].Signature;
  }

  for (int i = 0; i < Signature.argCount() && i < ArgRegs.size(); i++) {
    parseOperand(ArgRegs[i]);
  }
  if (Signature.argCount() > ArgRegs.size()) {
    CurrentAssembler->sub(x86::rsp, (Signature.argCount() - ArgRegs.size()) * 8);
    for (int i = 0; i < Signature.argCount() - ArgRegs.size(); i++) {
      parseOperand(x86::rax);
      CurrentAssembler->mov(x86::ptr(x86::rsp, i * 8), x86::rax);
    }
  }

  CurrentAssembler->sub(x86::rsp, 32);
  if (strcmp(FuncName, CurFunctionName) == 0) {
    CurrentAssembler->call(CurFunStartLabel);
  } else {
    CurrentAssembler->call(FuncMap[FuncName].Ptr);
  }
  CurrentAssembler->add(x86::rsp, 32);

  if (Signature.argCount() > ArgRegs.size()) {
    CurrentAssembler->add(x86::rsp, (Signature.argCount() - ArgRegs.size()) * 8);
  }

  if (Signature.ret() != TypeId::kVoid) {
    auto Mem = parseAssignableOperand();
    CurrentAssembler->mov(Mem, x86::rax);
  }
}

void JitInterpreter::parseGoto() {
  ++BufferPtr;
  auto Label = parseName();
  CurrentAssembler->jmp(getLabel(Label));
}

void JitInterpreter::parseBlock() {
  ++BufferPtr;
  auto Label = parseName();
  CurrentAssembler->bind(getLabel(Label));
}

void JitInterpreter::parseEQ() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentAssembler->cmp(x86::rax, x86::rbx);
  CurrentAssembler->je(getLabel(Label));
}

void JitInterpreter::parseNE() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentAssembler->cmp(x86::rax, x86::rbx);
  CurrentAssembler->jne(getLabel(Label));
}

void JitInterpreter::parseLT() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentAssembler->cmp(x86::rax, x86::rbx);
  CurrentAssembler->jl(getLabel(Label));
}

void JitInterpreter::parseLE() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentAssembler->cmp(x86::rax, x86::rbx);
  CurrentAssembler->jle(getLabel(Label));
}

void JitInterpreter::parseGT() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentAssembler->cmp(x86::rax, x86::rbx);
  CurrentAssembler->jg(getLabel(Label));
}

void JitInterpreter::parseGE() {
  ++BufferPtr;
  parseOperand(x86::rax);
  parseOperand(x86::rbx);
  auto Label = parseName();
  CurrentAssembler->cmp(x86::rax, x86::rbx);
  CurrentAssembler->jge(getLabel(Label));
}

Label JitInterpreter::getLabel(const char* LabelName) {
  if (Labels.find(LabelName) == Labels.end()) {
    Labels[LabelName] = CurrentAssembler->newLabel();
  }
  return Labels[LabelName];
}

void JitInterpreter::newVar(const char* Name) {
  VarIdx[Name] = CurIdx;
  CurrentAssembler->push(0);
  ++CurIdx;
}

void JitInterpreter::newVar(const char* Name, const x86::Gp& Value) {
  VarIdx[Name] = CurIdx;
  CurrentAssembler->push(Value);
  ++CurIdx;
}

x86::Mem JitInterpreter::getVarMem(const char* Name) {
  return x86::ptr(x86::rsp, (CurIdx - VarIdx[Name] - 1) * 8);
}

x86::Mem JitInterpreter::getVarMemByIdx(const char* Name, int64_t Idx) {
  CurrentAssembler->mov(x86::r15, getVarMem(Name));
  return x86::ptr(x86::r15, Idx * 8);
}

void JitInterpreter::assignVar(const char* Name, const x86::Gp& Src) {
  CurrentAssembler->mov(getVarMem(Name), Src);
}

void JitInterpreter::getValue(const char* Name, const x86::Gp& Dst) {
  CurrentAssembler->mov(Dst, getVarMem(Name));
}

void JitInterpreter::getValueByIdx(const char* Name, int64_t Idx, const x86::Gp& Dst) {
  CurrentAssembler->mov(x86::r15, getVarMem(Name));
  CurrentAssembler->mov(Dst, x86::ptr(x86::r15, Idx * 8));
}

int64_t* alloc(GarbageCollector* GC, int64_t Size) {
  return GC->newArrayAlloc(Size);
}

void JitInterpreter::allocNewArray(int64_t Size) {
  CurrentAssembler->mov(ArgRegs[0], &GC);
  CurrentAssembler->mov(ArgRegs[1], Size);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(alloc);
  CurrentAssembler->add(x86::rsp, 40);
}

void initGCStack(GarbageCollector* GC, int64_t* StackPtr) {
  GC->initStack(StackPtr);
}

void JitInterpreter::initGC() {
  CurrentAssembler->mov(ArgRegs[0], &GC);
  CurrentAssembler->mov(ArgRegs[1], x86::rsp);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(initGCStack);
  CurrentAssembler->add(x86::rsp, 40);
}

void callCollector(GarbageCollector* GC, int64_t* StackPtr) {
  GC->collect(StackPtr);
}

void JitInterpreter::callGC() {
  CurrentAssembler->mov(ArgRegs[0], &GC);
  CurrentAssembler->mov(ArgRegs[1], x86::rsp);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(callCollector);
  CurrentAssembler->add(x86::rsp, 40);
}
