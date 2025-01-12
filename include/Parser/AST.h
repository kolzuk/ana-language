#ifndef AST_H
#define AST_H

#include <llvm/ADT/StringMap.h>

#include <utility>
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;

class CompilationUnitAST;
class DeclarationAST;
class ConstantDeclaration;
class TypeAST;
class ExpressionAST;
class RelationAST;
class SimpleExpressionAST;
class AddOperatorAST;
class TermAST;
class MulOperatorAST;
class FactorAST;
class GetByIndexAST;
class IdentifierAST;
class IntegerLiteralAST;
class ExpressionFactorAST;

class ASTVisitor {
 public:
  virtual void visit(AST&) {};
  virtual void visit(CompilationUnitAST&) = 0;
  virtual void visit(DeclarationAST&) {};
  virtual void visit(ConstantDeclaration&) = 0;
  virtual void visit(TypeAST&) = 0;
  virtual void visit(ExpressionAST&) = 0;
  virtual void visit(RelationAST&) = 0;
  virtual void visit(SimpleExpressionAST&) = 0;
  virtual void visit(AddOperatorAST&) = 0;
  virtual void visit(TermAST&) = 0;
  virtual void visit(MulOperatorAST&) = 0;
  virtual void visit(FactorAST&) {};
  virtual void visit(GetByIndexAST&) = 0;
  virtual void visit(IdentifierAST&) = 0;
  virtual void visit(IntegerLiteralAST&) = 0;
  virtual void visit(ExpressionFactorAST&) = 0;
};

class AST {
 public:
  virtual ~AST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class CompilationUnitAST : public AST {
  using DeclarationsVector = llvm::SmallVector<DeclarationAST*, 8>;
  DeclarationsVector Declarations;
 public:
  DeclarationsVector getDeclarations() { return Declarations; }
  explicit CompilationUnitAST(DeclarationsVector Declarations) : Declarations(std::move(Declarations)) {}
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class DeclarationAST {
 public:
  virtual ~DeclarationAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class ConstantDeclaration : public DeclarationAST {
  TypeAST* T;
  IdentifierAST* Ident;
  ExpressionAST* Expr;
 public:
  explicit ConstantDeclaration(TypeAST* Type, IdentifierAST* Ident, ExpressionAST* Expression)
    : T(Type), Ident(Ident), Expr(Expression) {}

  TypeAST* getType() { return T; }
  IdentifierAST* getIdentifier() { return Ident; };
  ExpressionAST* getExpression() { return Expr; }

  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class TypeAST : public AST {
 public:
  enum TypeId : short {
    Integer,
    Array
  };
 private:
  TypeId TypeKind;
  TypeAST* ArrayType;
 public:
  TypeAST(TypeId TypeId, TypeAST* ArrayType)
    : TypeKind(TypeId), ArrayType(ArrayType) {}
  TypeId getTypeKind() { return TypeKind; }
  TypeAST* getArrayType() { return ArrayType; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionAST : public AST {
  SimpleExpressionAST* LHS;
  RelationAST* Rel;
  SimpleExpressionAST* RHS;

 public:
  explicit ExpressionAST(SimpleExpressionAST* LHS, RelationAST* Rel,
                         SimpleExpressionAST* RHS) : LHS(LHS), Rel(Rel), RHS(RHS) {}
  SimpleExpressionAST* getLHS() { return LHS; }
  RelationAST* getRel() { return Rel; }
  SimpleExpressionAST* getRHS() { return RHS; }

  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class RelationAST : public AST {
 public:
  enum RelationId : short {
    Equal,
    NotEqual,
    Less,
    LessEq,
    Greater,
    GreaterEq,
  };
 private:
  RelationId RelKind;
 public:
  explicit RelationAST(RelationId RelKind) : RelKind(RelKind) {}
  RelationId getRelKind() { return RelKind; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class SimpleExpressionAST : public AST {
 public:
  enum SignId : short {
    Minus,
    Plus
  };
 private:
  SignId SignKind;
  TermAST* Trm;
  llvm::SmallVector<AddOperatorAST*> AddOperators;
  llvm::SmallVector<TermAST*> Terms;

 public:
  explicit SimpleExpressionAST(
      SignId SignKind,
      TermAST* Trm,
      llvm::SmallVector<AddOperatorAST*> AddOperator,
      llvm::SmallVector<TermAST*> Terms)
      : SignKind(SignKind), Trm(Trm),
        AddOperators(std::move(AddOperator)), Terms(std::move(Terms)) {}

  SignId getSignKind() { return SignKind; }
  TermAST* getTrm() { return Trm; }
  llvm::SmallVector<AddOperatorAST*> getAddOperators() { return AddOperators; };
  llvm::SmallVector<TermAST*> getTerms() { return Terms; };
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class AddOperatorAST : public AST {
 public:
  enum AddOperatorId : short {
    Plus,
    Minus
  };
 private:
  AddOperatorId AddOperatorKind;
 public:
  explicit AddOperatorAST(AddOperatorId AddOperatorKind) : AddOperatorKind(AddOperatorKind) {}
  AddOperatorId getAddOperatorKind() { return AddOperatorKind; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class TermAST : public AST {
  FactorAST* Fctor;
  llvm::SmallVector<MulOperatorAST*> MulOperators;
  llvm::SmallVector<FactorAST*> Factors;
 public:
  explicit TermAST(
      FactorAST* Fctor,
      llvm::SmallVector<MulOperatorAST*> MulOperators,
      llvm::SmallVector<FactorAST*> Factors)
    : Fctor(Fctor), MulOperators(std::move(MulOperators)), Factors(std::move(Factors)) {}

  FactorAST* getFctor() { return Fctor; };
  llvm::SmallVector<MulOperatorAST*> getMulOperators() { return MulOperators; }
  llvm::SmallVector<FactorAST*> getFactors() { return Factors; } ;
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class MulOperatorAST : public AST {
 public:
  enum MulOperatorId : short {
    Multiple,
    Divide
  };
 private:
  MulOperatorId MulOperatorKind;
 public:
  explicit MulOperatorAST(MulOperatorId MulOperatorKind) : MulOperatorKind(MulOperatorKind) {}
  MulOperatorId getMulOperatorKind() { return MulOperatorKind; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class FactorAST : public AST {
 public:
  virtual ~FactorAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class GetByIndexAST : public FactorAST {
  IdentifierAST* BaseIdentifier;
  ExpressionAST* Expr;
 public:
  explicit GetByIndexAST(IdentifierAST* BaseIdentifier, ExpressionAST* Expr)
    : BaseIdentifier(BaseIdentifier), Expr(Expr) {}

  IdentifierAST* getIdentifier() { return BaseIdentifier; }
  ExpressionAST* getExpr() { return Expr; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IdentifierAST : public FactorAST {
  llvm::StringRef Value;
 public:
  explicit IdentifierAST(llvm::StringRef Value) : Value(Value) {}
  llvm::StringRef getValue() { return Value; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IntegerLiteralAST : public FactorAST {
  llvm::StringRef Value;
 public:
  explicit IntegerLiteralAST(llvm::StringRef Value) : Value(Value) {}
  llvm::StringRef getValue() { return Value; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionFactorAST : public FactorAST {
  ExpressionAST* Expr;
 public:
  explicit ExpressionFactorAST(ExpressionAST* Expr) : Expr(Expr) {}

  ExpressionAST* getExpr() { return Expr; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

#endif //AST_H
