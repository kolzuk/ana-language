#include "Codegen/CodeGen.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

class ToIRVisitor : public ASTVisitor {
  Module* M;
  IRBuilder<> Builder;
  Type* VoidTy;
  Type* Int64Ty;
  ArrayType* ArrayTy;
  Type* PtrTy;
  Value* V = nullptr;
  Function* CurrentFunction = nullptr;
  StringMap<Function*> FunctionsMap;
  StringMap<Value*> NameMap;
  DenseMap<Value*, StringRef> ValueMap;
  StringMap<Type*> TypeMap;
  Function* PrintFunction;
  Align Int64Align = Align(8);
 public:
  ToIRVisitor(Module* M) : M(M), Builder(M->getContext()) {
    VoidTy = Type::getVoidTy(M->getContext());
    Int64Ty = Type::getInt64Ty(M->getContext());
    PtrTy = PointerType::get(Int64Ty, 0);
  }

  void run(AST* Tree) {
    PrintFunction = Function::Create(
        FunctionType::get(
            Type::getInt32Ty(M->getContext()),  // Возвращаемое значение printf
            PointerType::get(Type::getInt8Ty(M->getContext()), 0), // char*
            true), // printf имеет переменное количество аргументов
        Function::ExternalLinkage,
        "printf",
        M);

    Tree->accept(*this);
  }

  void visit(CompilationUnitAST& Node) override {
    for (auto Declaration : Node.Declarations) {
      Declaration->accept(*this);
    }
  }

  void visit(VariableDeclarationAST& Node) override {
    TypeAST::TypeKind Type = Node.T->Type;
    llvm::StringRef Name = Node.Ident->Value;

    AllocaInst* Alloca = nullptr;
    if (Type == TypeAST::Array) {
      Node.T->accept(*this);

      Alloca = Builder.CreateAlloca(ArrayTy, V, Name);
      TypeMap[Name] = ArrayTy;
      if (Node.Expr) {
        Node.Expr->accept(*this);
        Builder.CreateStore(V, Alloca);
      }
    } else if (Type == TypeAST::Integer) {
      Alloca = Builder.CreateAlloca(Int64Ty, nullptr, Name);
      Alloca->setAlignment(Int64Align);
      TypeMap[Name] = Int64Ty;
      if (Node.Expr) {
        Node.Expr->accept(*this);
        Builder.CreateStore(getValue(V), Alloca)->setAlignment(Int64Align);
      } else {
        Builder.CreateStore(ConstantInt::get(Int64Ty, 0l), Alloca)->setAlignment(Int64Align);
      }
    }

    NameMap[Name] = Alloca;
    ValueMap[Alloca] = Name;
  }

  virtual void visit(FunctionDeclarationAST& Node) override {
    SmallVector<Type*> ArgTypes;
    for (auto* ArgType : Node.Arguments->Types)
      ArgTypes.push_back(convertType(ArgType));

    FunctionType* FunctionTy =
        FunctionType::get(convertType(Node.ReturnType), ArgTypes, false);

    llvm::StringRef FunctionName = Node.Ident->Value;
    CurrentFunction = Function::Create(
        FunctionTy, GlobalValue::ExternalLinkage, FunctionName, M);
    NameMap[FunctionName] = CurrentFunction;
    TypeMap[FunctionName] = FunctionTy;
    BasicBlock* BB = BasicBlock::Create(M->getContext(), "entry", CurrentFunction);
    Builder.SetInsertPoint(BB);

    unsigned Idx = 0;
    for (auto& Arg : CurrentFunction->args()) {
      llvm::StringRef ArgName = Node.Arguments->Idents[Idx]->Value;
      Arg.setName(ArgName);

      AllocaInst* Alloca = nullptr;
      switch (Node.Arguments->Types[Idx]->Type) {
        case TypeAST::Array:Alloca = Builder.CreateAlloca(PtrTy, nullptr);
          TypeMap[ArgName] = PtrTy;
          break;
        case TypeAST::Integer:Alloca = Builder.CreateAlloca(Int64Ty, nullptr);
          TypeMap[ArgName] = Int64Ty;
          break;
        case TypeAST::Void:break;
      }

      Builder.CreateStore(&Arg, Alloca)->setAlignment(Int64Align);
      NameMap[ArgName] = Alloca;
      ++Idx;
    }

    Node.Body->accept(*this);
  }

  virtual void visit(StatementSequenceAST& Node) override {
    for (auto Declaration : Node.Statements) {
      Declaration->accept(*this);
    }
  }

  virtual void visit(IfStatementAST& Node) override {
    Node.Condition->accept(*this);
    Value* CondResult = V;

    llvm::BasicBlock* IfBodyBB = llvm::BasicBlock::Create(
        M->getContext(), "if.body", CurrentFunction);
    llvm::BasicBlock* ElseBodyBB = llvm::BasicBlock::Create(
        M->getContext(), "else.body", CurrentFunction);
    llvm::BasicBlock* AfterIfBB = llvm::BasicBlock::Create(
        M->getContext(), "after.if", CurrentFunction);

    Builder.CreateCondBr(CondResult, IfBodyBB, ElseBodyBB);

    Builder.SetInsertPoint(IfBodyBB);
    Node.Body->accept(*this);
    Builder.CreateBr(AfterIfBB);

    Builder.SetInsertPoint(ElseBodyBB);
    if (Node.ElseBody) {
      Node.Body->accept(*this);
    }
    Builder.CreateBr(AfterIfBB);

    Builder.SetInsertPoint(AfterIfBB);
  }
  virtual void visit(WhileStatementAST& Node) override {
    Node.Condition->accept(*this);
    Value* CondResult = V;

    llvm::BasicBlock* WhileBodyBB = llvm::BasicBlock::Create(
        M->getContext(), "while.body", CurrentFunction);
    llvm::BasicBlock* AfterWhileBB = llvm::BasicBlock::Create(
        M->getContext(), "after.while", CurrentFunction);

    Builder.CreateCondBr(CondResult, WhileBodyBB, AfterWhileBB);

    Builder.SetInsertPoint(WhileBodyBB);
    Node.Body->accept(*this);
    Node.Condition->accept(*this);
    CondResult = V;
    Builder.CreateCondBr(CondResult, WhileBodyBB, AfterWhileBB);

    Builder.SetInsertPoint(AfterWhileBB);
  }

  virtual void visit(ReturnStatementAST& Node) override {
    if (Node.Expr == nullptr) {
      Builder.CreateRetVoid();
      return;
    }
    Node.Expr->accept(*this);
    Builder.CreateRet(getValue(V));
  }

  virtual void visit(AssignStatementAST& Node) override {
    Node.LHS->accept(*this);
    auto* LHS = V;

    if (Node.RHS) {
      Node.RHS->accept(*this);
      Builder.CreateStore(getValue(V), LHS)->setAlignment(Int64Align);
    }
  }

  virtual void visit(PrintStatementAST& Node) override {
    auto FormatStr = Builder.CreateGlobalStringPtr("%ld\n");
    Node.Expr->accept(*this);
    Builder.CreateCall(PrintFunction, {FormatStr, getValue(V)});
  }

  virtual void visit(IntegerTypeAST&) override {
  }

  virtual void visit(ArrayTypeAST& Node) override {
    Node.Size->accept(*this);
    Value* SizeValue = V;

    ArrayTy = ArrayType::get(Int64Ty, cast<ConstantInt>(SizeValue)->getZExtValue());
  }

  virtual void visit(ArgumentsListAST&) override {
  }
  virtual void visit(ExpressionsListAST&) override {
  }

  void visit(ExpressionAST& Node) override {
    Node.LHS->accept(*this);
    Value* LHS = V;
    if (Node.RHS) {
      Node.RHS->accept(*this);
      Value* RHS = getValue(V);
      LHS = getValue(LHS);

      switch (Node.Rel->RelKind) {
        case RelationAST::Equal:V = Builder.CreateICmpEQ(LHS, RHS);
          break;
        case RelationAST::NotEqual:V = Builder.CreateICmpNE(LHS, RHS);
          break;
        case RelationAST::Less:V = Builder.CreateICmpSLT(LHS, RHS);
          break;
        case RelationAST::LessEq:V = Builder.CreateICmpSLE(LHS, RHS);
          break;
        case RelationAST::Greater:V = Builder.CreateICmpSGT(LHS, RHS);
          break;
        case RelationAST::GreaterEq:V = Builder.CreateICmpSGE(LHS, RHS);
          break;
      }
    } else {
      V = LHS;
    }
  }

  void visit(RelationAST&) override {
  }

  void visit(SimpleExpressionAST& Node) override {
    Node.Trm->accept(*this);
    Value* Result = V;
    auto AddOps = Node.AddOperators;
    auto Terms = Node.Terms;
    for (size_t i = 0; i < AddOps.size(); ++i) {
      Terms[i]->accept(*this);
      Value* TermVal = V;
      switch (AddOps[i]->AddOperatorKind) {
        case AddOperatorAST::Plus:Result = Builder.CreateAdd(getValue(Result), getValue(TermVal));
          break;
        case AddOperatorAST::Minus:Result = Builder.CreateSub(getValue(Result), getValue(TermVal));
          break;
      }
    }

    V = Result;
  }

  void visit(AddOperatorAST&) override {
  }

  void visit(TermAST& Node) override {
    Node.MulOperand->accept(*this);
    Value* Result = V;
    auto MulOps = Node.MulOperators;
    auto Factors = Node.MulOperands;
    for (size_t i = 0; i < MulOps.size(); ++i) {
      Factors[i]->accept(*this);
      Value* FactorVal = V;
      switch (MulOps[i]->MulOperatorKind) {
        case MulOperatorAST::Multiple:Result = Builder.CreateMul(getValue(Result), getValue(FactorVal));
          break;
        case MulOperatorAST::Divide:Result = Builder.CreateSDiv(getValue(Result), getValue(FactorVal));
          break;
      }
    }

    V = Result;
  }

  void visit(MulOperatorAST&) override {
  }

  void visit(MulOperandAST& Node) override {
    Node.Factor->accept(*this);
    if (Node.Operator) {
      switch (Node.Operator->Kind) {
        case UnaryOperatorAST::UnaryOperatorKind::Minus:
          V = Builder.CreateMul(ConstantInt::get(Int64Ty, -1l),
                                getValue(V));
          break;
        case UnaryOperatorAST::Plus:break;
      }
    }
  }

  void visit(UnaryOperatorAST&) override {
  }

  void visit(IdentifierAST& Node) override {
    V = NameMap[Node.Value];
  }

  void visit(IntegerLiteralAST& Node) override {
    V = ConstantInt::get(Int64Ty, std::stoll(Node.Value.str()));
  }

  void visit(ArrayInitializationAST& Node) override {
    SmallVector<Constant*> Elements;
    for (auto Expr : Node.Exprs) {
      Expr->accept(*this);
      if (auto* ConstVal = dyn_cast<Constant>(V)) {
        Elements.push_back(ConstVal);
      } else {
        llvm::errs() << "Non-constant value in array initialization.\n";
        return;
      }
    }

    ArrayTy = ArrayType::get(Int64Ty, Elements.size());
    V = ConstantArray::get(ArrayTy, ArrayRef(Elements));
  }

  void visit(GetByIndexAST& Node) override {
    llvm::StringRef ArrayName = Node.Ident->Value;

    Node.Index->accept(*this);
    auto* Index = V;

    // TODO
//    auto* Ptr = Builder.CreateInBoundsGEP(ArrayTy, NameMap[ArrayName],
//    {ConstantInt::get(Int64Ty, 0), Index});
    auto* LoadedPointer = Builder.CreateLoad(PtrTy, NameMap[ArrayName]);
    auto* Ptr = Builder.CreateInBoundsGEP(Int64Ty, LoadedPointer, {Index});
    V = Builder.CreateLoad(Int64Ty, Ptr);
  }

  void visit(ExpressionFactorAST& Node) override {
    Node.Expr->accept(*this);
  }

  void visit(FunctionCallAST& Node) override {
    auto FunctionName = Node.Ident->Value;
    auto* CallableFunction = dyn_cast<Function>(NameMap[FunctionName]);
    llvm::SmallVector<Value*> Params;

    for (auto Param : Node.ExprList->Exprs) {
      Param->accept(*this);
      Params.push_back(V);
    }

    V = Builder.CreateCall(CallableFunction->getFunctionType(),
                           CallableFunction,
                           Params);
  }

  Type* convertType(TypeAST* Ty) {
    switch (Ty->Type) {
      case TypeAST::Integer:return Int64Ty;
      case TypeAST::Array:return PtrTy;
      case TypeAST::Void:return VoidTy;
    }
  }

 private:
  Value* getValue(Value* Value, llvm::Type* Type, Align Align) {
    if (Value->getType() != PtrTy) {
      return Value;
    }
    auto Load = Builder.CreateLoad(Type, Value);
    Load->setAlignment(Align);
    return Load;
  }

  Value* getValue(Value* Value) {
    return getValue(Value, Int64Ty, Int64Align);
  }
};
}

void CodeGen::compile(AST* Tree, const std::string& SourceFilename) {
  LLVMContext Ctx;
  auto* M = new Module(SourceFilename, Ctx);
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);

  std::string OutputFilename = SourceFilename + ".ll";
  std::error_code EC;
  sys::fs::OpenFlags OpenFlags = sys::fs::OF_None;
  auto Out = std::make_unique<llvm::ToolOutputFile>(
      OutputFilename, EC, OpenFlags);
  if (EC) {
    llvm::errs() << EC.message() << '\n';
    return;
  }
  M->print(Out->os(), nullptr);
  Out->keep();
}