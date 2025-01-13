#include "Codegen/Codegen.h"

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
  Type* Int32Ty;
  PointerType* PtrTy;
  Constant* Int32Zero;
  Value* V;
  StringMap<Value*> NameMap;
 public:
  ToIRVisitor(Module* M) : M(M), Builder(M->getContext()) {
    VoidTy = Type::getVoidTy(M->getContext());
    Int32Ty = Type::getInt32Ty(M->getContext());
    PtrTy = PointerType::getUnqual(M->getContext());
    Int32Zero = ConstantInt::get(Int32Ty, 0, true);
  }

  void run(AST* Tree) {
    FunctionType* MainFty = FunctionType::get(
        Int32Ty, {Int32Ty, PtrTy}, false);
    Function* MainFn = Function::Create(
        MainFty, GlobalValue::ExternalLinkage,
        "main", M);

    BasicBlock* BB = BasicBlock::Create(M->getContext(),
                                        "entry", MainFn);
    Builder.SetInsertPoint(BB);

    Tree->accept(*this);

    Builder.CreateRet(V);
  }

  virtual void visit(CompilationUnitAST& Node) override {
    for (auto Decl : Node.getDeclarations()) {
      Decl->accept(*this);
    }
  }

  virtual void visit(ConstantDeclaration& Node) override {
    IdentifierAST* Ident = Node.getIdentifier();
    ExpressionAST* Expr = Node.getExpression();

    Expr->accept(*this);

    auto* Alloca = Builder.CreateAlloca(Int32Ty, nullptr, Ident->getValue());
    Builder.CreateStore(V, Alloca);

    NameMap[Ident->getValue()] = Alloca;
  }

  virtual void visit(TypeAST& Node) override {
  }

  virtual void visit(ExpressionAST& Node) override {
    Node.getLHS()->accept(*this);
    Value* LHS = V;

    if (Node.getRHS()) {
      Node.getRHS()->accept(*this);
      Value* RHS = V;

      switch (Node.getRel()->getRelKind()) {
        case RelationAST::Equal:
          V = Builder.CreateICmpEQ(LHS, RHS);
          break;
        case RelationAST::NotEqual:
          V = Builder.CreateICmpNE(LHS, RHS);
          break;
        case RelationAST::Less:
          V = Builder.CreateICmpSLT(LHS, RHS);
          break;
        case RelationAST::LessEq:
          V = Builder.CreateICmpSLE(LHS, RHS);
          break;
        case RelationAST::Greater:
          V = Builder.CreateICmpSGT(LHS, RHS);
          break;
        case RelationAST::GreaterEq:
          V = Builder.CreateICmpSGE(LHS, RHS);
          break;
      }
    } else {
      V = LHS;
    }
  }

  virtual void visit(RelationAST& Node) override {
  }

  virtual void visit(SimpleExpressionAST& Node) override {
    Node.getTrm()->accept(*this);
    Value* Result = V;

    auto AddOps = Node.getAddOperators();
    auto Terms = Node.getTerms();
    for (size_t i = 0; i < AddOps.size(); ++i) {
      Terms[i]->accept(*this);
      Value* TermVal = V;

      switch (AddOps[i]->getAddOperatorKind()) {
        case AddOperatorAST::Plus:
          Result = Builder.CreateAdd(Result, TermVal);
          break;
        case AddOperatorAST::Minus:
          Result = Builder.CreateSub(Result, TermVal);
          break;
      }
    }

    V = Result;
  }

  virtual void visit(AddOperatorAST& Node) override {
  }

  virtual void visit(TermAST& Node) override {
    Node.getFctor()->accept(*this);
    Value* Result = V;

    auto MulOps = Node.getMulOperators();
    auto Factors = Node.getFactors();
    for (size_t i = 0; i < MulOps.size(); ++i) {
      Factors[i]->accept(*this);
      Value* FactorVal = V;

      switch (MulOps[i]->getMulOperatorKind()) {
        case MulOperatorAST::Multiple:
          Result = Builder.CreateMul(Result, FactorVal);
          break;
        case MulOperatorAST::Divide:
          Result = Builder.CreateSDiv(Result, FactorVal);
          break;
      }
    }

    V = Result;
  }

  virtual void visit(MulOperatorAST& Node) override {
  }

  virtual void visit(GetByIndexAST& Node) override {
    Node.getExpr()->accept(*this);
    Value* Index = V;
    llvm::StringRef Name = Node.getIdentifier()->getValue();
    Value* Base = NameMap[Name];
    V = Builder.CreateGEP(Int32Ty, Base, Index);
  }

  virtual void visit(IdentifierAST& Node) override {
    V = Builder.CreateLoad(Int32Ty, NameMap[Node.getValue()]);
  }

  virtual void visit(IntegerLiteralAST& Node) override {
    V = ConstantInt::get(Int32Ty, std::stoi(Node.getValue().str()));
  }

  virtual void visit(ExpressionFactorAST& Node) override {
    Node.getExpr()->accept(*this);
  }

  virtual void visit(IntegerTypeAST& Node) override {
  }

  virtual void visit(ArrayTypeAST& Node) override {
  }
};
}

void CodeGen::compile(AST* Tree, std::string& SourceFilename) {
  LLVMContext Ctx;
  auto* M = new Module(SourceFilename, Ctx);
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);

  auto OutputFilename = SourceFilename + ".ll";
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