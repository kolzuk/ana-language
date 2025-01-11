#ifndef AST_H
#define AST_H

#include <llvm/ADT/StringMap.h>

#include <utility>
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class StatementsSequence;
class Statement;
class Declaration;
class Expr;
class Factor;
class BinaryOp;

class ASTVisitor {
 public:
  virtual void visit(AST&) {};
  virtual void visit(StatementsSequence&) = 0;
  virtual void visit(Statement&) {};
  virtual void visit(Declaration&) = 0;
  virtual void visit(Expr&) {};
  virtual void visit(Factor&) = 0;
  virtual void visit(BinaryOp&) = 0;
};

class AST {
 public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor& V) = 0;
};

class StatementsSequence : public AST {
  using StatementsVector = llvm::SmallVector<AST*, 8>;
  StatementsVector Statements;
 public:
  StatementsVector getStatements() { return Statements; }
  explicit StatementsSequence(StatementsVector Statements) : Statements(std::move(Statements)) {}
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Statement : public AST {
 public:
  Statement() {}
};

class Declaration : public Statement {
  llvm::StringRef Identifier;
  AST* E;
 public:
  Declaration(llvm::StringRef Identifier, AST* E)
      : Identifier(Identifier), E(E) {
  }
  llvm::StringRef getIdentifier() { return Identifier; };
  AST* getExpr() { return E; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Expr : public AST {
 public:
  Expr() {}
};

class Factor : public AST {
 public:
  enum ValueKind { Ident, Number };
 private:
  ValueKind Kind;
  llvm::StringRef Val;
 public:
  Factor(ValueKind Kind, llvm::StringRef Val)
      : Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class BinaryOp : public AST {
 public:
  enum Operator { Plus, Minus, Mul, Div };
 private:
  AST* Left;
  AST* Right;
  Operator Op;
 public:
  BinaryOp(Operator Op, AST* L, AST* R)
      : Op(Op), Left(L), Right(R) {}
  AST* getLeft() { return Left; }
  AST* getRight() { return Right; }
  Operator getOperator() { return Op; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

#endif //AST_H
