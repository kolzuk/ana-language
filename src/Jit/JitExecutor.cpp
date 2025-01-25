#include "Jit/JitExecutor.h"

#ifdef _WIN64
const std::vector<x86::Gp> ArgRegs{x86::rcx, x86::rdx, x86::r8, x86::r9};
#else
const std::vector<x86::Gp> ArgRegs { x86::rdi, x86::rsi, x86::rdx, x86::rcx, x86::r8, x86::r9 };
#endif

Func JitExecutor::lookupMain() {
  return FuncMap["main"].Ptr;
}

void JitExecutor::addFunction(std::vector<std::string> Names) {
  resetCompiler();
  FuncSignature Signature(CallConvId::kHost);
  Signature.setRet(TypeId::kInt64);
  CurFunctionName = Names[0];

  for (int i = 1; i < Names.size(); i++) {
    Signature.addArg(TypeId::kInt64);
  }

  CurFunctionSignature = Signature;

  Label Start = CurrentAssembler->newLabel();
  CurrentAssembler->bind(Start);
  CurFunStartLabel = Start;
  for (int i = 1; i < Names.size(); i++) {
    if (i - 1 < ArgRegs.size()) {
      newVar(Names[i], ArgRegs[i - 1]);
    } else {
      newVar(Names[i]);
      CurrentAssembler->mov(x86::rax, x86::ptr(x86::rsp, (CurIdx + i) * 8));
      assignVar(Names[i], x86::rax);
    }
  }
}

void JitExecutor::endFunction() {
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

  FuncMap[CurFunctionName] = NativeFunction{Function, CurFunctionSignature};
}

void JitExecutor::addFunctionCall(const std::string& Name) {
  FuncSignature Signature;

  if (Name == CurFunctionName) {
    Signature = CurFunctionSignature;
  } else {
    Signature = FuncMap[Name].Signature;
  }

  for (int i = 0; i < Signature.argCount() && i < ArgRegs.size(); i++) {
    popStack(ArgRegs[i]);
  }

  CurrentAssembler->sub(x86::rsp, 32);
  if (Name == CurFunctionName) {
    CurrentAssembler->call(CurFunStartLabel);
  } else {
    CurrentAssembler->call(FuncMap[Name].Ptr);
  }
  CurrentAssembler->add(x86::rsp, 32);

  if (Signature.argCount() > ArgRegs.size()) {
    CurrentAssembler->add(x86::rsp, (Signature.argCount() - ArgRegs.size()) * 8);
    CurIdx += Signature.argCount() - ArgRegs.size();
  }

  pushStack(x86::rax);
}

void cPrint(int64_t x) {
  printf("%lld\n", x);
}

void JitExecutor::print() {
  popStack(ArgRegs[0]);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(cPrint);
  CurrentAssembler->add(x86::rsp, 40);
}

void JitExecutor::addReturn() {
  popStack(x86::rax);
  CurrentAssembler->add(x86::rsp, CurIdx * 8);
  CurrentAssembler->ret();
}

void JitExecutor::add() {
  popStack(x86::rax);
  popStack(x86::rbx);
  CurrentAssembler->add(x86::rax, x86::rbx);
  pushStack(x86::rax);
}

void JitExecutor::sub() {
  popStack(x86::rax);
  popStack(x86::rbx);
  CurrentAssembler->sub(x86::rax, x86::rbx);
  pushStack(x86::rax);
}

void JitExecutor::mul() {
  popStack(x86::rax);
  popStack(x86::rbx);
  CurrentAssembler->cqo(x86::rdx, x86::rax);
  CurrentAssembler->xor_(x86::r8, x86::r8);
  CurrentAssembler->xor_(x86::r9, x86::r9);
  CurrentAssembler->imul(x86::r8, x86::r9, x86::rbx);
  pushStack(x86::rax);
}

void JitExecutor::div() {
  popStack(x86::rax);
  popStack(x86::rbx);
  CurrentAssembler->cqo(x86::rdx, x86::rax);
  CurrentAssembler->xor_(x86::r8, x86::r8);
  CurrentAssembler->xor_(x86::r9, x86::r9);
  CurrentAssembler->idiv(x86::r8, x86::r9, x86::rbx);
  pushStack(x86::rax);
}

void JitExecutor::mod() {
  popStack(x86::rax);
  popStack(x86::rbx);
  CurrentAssembler->cqo(x86::rdx, x86::rax);
  CurrentAssembler->xor_(x86::r8, x86::r8);
  CurrentAssembler->xor_(x86::r9, x86::r9);
  CurrentAssembler->idiv(x86::r8, x86::r9, x86::rbx);
  pushStack(x86::rdx);
}

void JitExecutor::push(int64_t x) {
  pushStack(x);
}

void JitExecutor::assign(const std::string& Name) {
  popStack(x86::rax);
  if (VarIdx.find(Name) == VarIdx.end()) {
    newVar(Name, x86::rax);
  } else {
    assignVar(Name, x86::rax);
  }
}

void JitExecutor::assignByIndex(const std::string& Name) {
  popStack(x86::rax);
  popStack(x86::rbx);
  assignVarByIdx(Name, x86::rbx, x86::rax);
}

x86::Mem JitExecutor::getVarMem(const std::string& Name) {
  return x86::ptr(x86::rsp, (CurIdx - VarIdx[Name] - 1) * 8);
}

x86::Mem JitExecutor::getVarMemByIdx(const std::string& Name, const x86::Gp& Idx) {
  CurrentAssembler->mov(x86::r15, getVarMem(Name));
  return x86::ptr(x86::r15, Idx, 3);
}

void JitExecutor::assignVar(const std::string& Name, const x86::Gp& Src) {
  CurrentAssembler->mov(getVarMem(Name), Src);
}

void JitExecutor::assignVarByIdx(const std::string& Name, const x86::Gp& Idx, const x86::Gp& Src) {
  CurrentAssembler->mov(getVarMemByIdx(Name, Idx), Src);
}

void JitExecutor::getValue(const std::string& Name, const x86::Gp& Dst) {
  CurrentAssembler->mov(Dst, getVarMem(Name));
}

void JitExecutor::getValueByIdx(const std::string& Name, const x86::Gp& Idx, const x86::Gp& Dst) {
  CurrentAssembler->mov(Dst, getVarMemByIdx(Name, Idx));
}

int64_t* alloc(GarbageCollector* GC, int64_t Size) {
  return GC->newArrayAlloc(Size);
}

void JitExecutor::allocNewArray(const std::string& Name) {
  popStack(x86::rax);
  CurrentAssembler->mov(ArgRegs[0], &GC);
  CurrentAssembler->mov(ArgRegs[1], x86::rax);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(alloc);
  CurrentAssembler->add(x86::rsp, 40);
  newVar(Name, x86::rax);
}

void initGCStack(GarbageCollector* GC, int64_t* StackPtr) {
  GC->initStack(StackPtr);
}

void JitExecutor::initGC() {
  CurrentAssembler->mov(ArgRegs[0], &GC);
  CurrentAssembler->mov(ArgRegs[1], x86::rsp);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(initGCStack);
  CurrentAssembler->add(x86::rsp, 40);
}

void callCollector(GarbageCollector* GC, int64_t* StackPtr) {
  GC->collect(StackPtr);
}

void JitExecutor::callGC() {
  CurrentAssembler->mov(ArgRegs[0], &GC);
  CurrentAssembler->mov(ArgRegs[1], x86::rsp);
  CurrentAssembler->sub(x86::rsp, 40);
  CurrentAssembler->call(callCollector);
  CurrentAssembler->add(x86::rsp, 40);
}

Label JitExecutor::getLabel(const std::string& LabelName) {
  if (Labels.find(LabelName) == Labels.end()) {
    Labels[LabelName] = CurrentAssembler->newLabel();
  }
  return Labels[LabelName];
}

void JitExecutor::newVar(const std::string& Name) {
  VarIdx[Name] = CurIdx;
  pushStack(0);
}

void JitExecutor::newVar(const std::string& Name, const x86::Gp& Value) {
  VarIdx[Name] = CurIdx;
  pushStack(Value);
}

void JitExecutor::load(const std::string& Name) {
  getValue(Name, x86::rax);
  pushStack(x86::rax);
}

void JitExecutor::loadByIndex(const std::string& Name) {
  popStack(x86::rbx);
  getValueByIdx(Name, x86::rbx, x86::rax);
  pushStack(x86::rax);
}

void JitExecutor::addGoto(const std::string& Label) {
  CurrentAssembler->jmp(getLabel(Label));
}

void JitExecutor::label(const std::string& Label) {
  CurrentAssembler->bind(getLabel(Label));
}

void JitExecutor::cmp() {
  popStack(x86::rax);
  popStack(x86::rbx);
  CurrentAssembler->cmp(x86::rax, x86::rbx);
}

void JitExecutor::jumpEq(const std::string& Label) {
  CurrentAssembler->je(getLabel(Label));
}

void JitExecutor::jumpNe(const std::string& Label) {
  CurrentAssembler->jne(getLabel(Label));
}

void JitExecutor::jumpLt(const std::string& Label) {
  CurrentAssembler->jl(getLabel(Label));
}

void JitExecutor::jumpLe(const std::string& Label) {
  CurrentAssembler->jle(getLabel(Label));
}

void JitExecutor::jumpGt(const std::string& Label) {
  CurrentAssembler->jg(getLabel(Label));
}

void JitExecutor::jumpGe(const std::string& Label) {
  CurrentAssembler->jge(getLabel(Label));
}
