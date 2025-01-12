#ifndef AST_H
#define AST_H

#include <llvm/ADT/StringMap.h>

#include <utility>
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;

class CompilationUnit;
class Declaration;
class VariableDeclaration;
class Type;
class StatementsSequence;
class Statement;
class AssignStatement;
class Expression;
class Relation;
class SimpleExpression;
class AddOperator;
class Term;
class MulOperator;
class Factor;
class FunctionCall;
class ExpressionList;
class GetByIndex;
class Identifier;
class IntegerLiteral;

class ASTVisitor {
 public:
  virtual void visit(AST&) {};
  virtual void visit(CompilationUnit&) = 0;
  virtual void visit(Declaration&) {};
  virtual void visit(VariableDeclaration&) = 0;
  virtual void visit(Type&) = 0;
  virtual void visit(StatementsSequence&) = 0;
  virtual void visit(Statement&) {};
  virtual void visit(AssignStatement&) = 0;
  virtual void visit(Expression&) = 0;
  virtual void visit(Relation&) = 0;
  virtual void visit(SimpleExpression&) = 0;
  virtual void visit(AddOperator&) = 0;
  virtual void visit(Term&) = 0;
  virtual void visit(MulOperator&) = 0;
  virtual void visit(Factor&) {};
  virtual void visit(FunctionCall&) = 0;
  virtual void visit(ExpressionList&) = 0;
  virtual void visit(GetByIndex&) = 0;
  virtual void visit(Identifier&) = 0;
  virtual void visit(IntegerLiteral&) = 0;
};

class AST {
 public:
  virtual ~AST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class CompilationUnit : public AST {
  using DeclarationsVector = llvm::SmallVector<AST*, 8>;
  DeclarationsVector Declarations;
 public:
  DeclarationsVector getDeclarations() { return Declarations; }
  explicit CompilationUnit(DeclarationsVector Declarations) : Declarations(std::move(Declarations)) {}
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Declaration {
 public:
  virtual ~Declaration() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class VariableDeclaration : public AST {
  Type* T;
  Identifier* Ident;
  Expression* Expr;
 public:
  explicit VariableDeclaration(Type* Type, Identifier* Ident, Expression* Expression)
    : T(Type), Ident(Ident), Expr(Expression) {}

  Type* getType() { return T; }
  Identifier* getIdentifier() { return Ident; };
  Expression* getExpression() { return Expr; }

  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Type : public AST {
 public:
  enum TypeId : short {
    Integer,
    Array
  };
 private:
  TypeId TypeKind;
  Type* ArrayType;
 public:
  Type(TypeId TypeId, Type* ArrayType)
    : TypeKind(TypeId), ArrayType(ArrayType) {}
  TypeId getTypeKind() { return TypeKind; }
  Type* getArrayType() { return ArrayType; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class StatementsSequence : public AST {
  using StatementsVector = llvm::SmallVector<AST*, 8>;
  StatementsVector Statements;
 public:
  explicit StatementsSequence(StatementsVector Statements) : Statements(std::move(Statements)) {}
  StatementsVector getStatements() { return Statements; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Statement : public AST {
 public:
  virtual ~Statement() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class AssignStatement : public AST {
  Identifier* Ident;
  llvm::SmallVector<Expression*> ArraysIndexes;
  Expression* AssignExpression;
 public:
  explicit AssignStatement(Identifier* Ident,
    llvm::SmallVector<Expression*> ArraysIndexes,
    Expression* AssignExpression)
    : Ident(Ident),
      ArraysIndexes(std::move(ArraysIndexes)),
      AssignExpression(AssignExpression) {}
  Identifier* getIdentifier() { return Ident; }
  llvm::SmallVector<Expression*> getArraysIndexes() { return ArraysIndexes; }
  Expression* getAssignExpression() { return AssignExpression; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Expression : public AST {
  SimpleExpression* LHS;
  Relation* Rel;
  SimpleExpression* RHS;

 public:
  explicit Expression(SimpleExpression* LHS, Relation* Rel,
                      SimpleExpression* RHS) : LHS(LHS), Rel(Rel), RHS(RHS) {}
  SimpleExpression* getLHS() { return LHS; }
  Relation* getRel() { return Rel; }
  SimpleExpression* getRHS() { return RHS; }

  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Relation : public AST {
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
  explicit Relation(RelationId RelKind) : RelKind(RelKind) {}
  RelationId getRelKind() { return RelKind; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class SimpleExpression : public AST {
 public:
  enum SignId : short {
    Minus,
    Plus
  };
 private:
  SignId SignKind;
  Term* Trm;
  llvm::SmallVector<AddOperator*> AddOperators;
  llvm::SmallVector<Term*> Terms;

 public:
  explicit SimpleExpression(
      SignId SignKind,
      Term* Trm,
      llvm::SmallVector<AddOperator*> AddOperator,
      llvm::SmallVector<Term*> Terms)
      : SignKind(SignKind), Trm(Trm),
        AddOperators(std::move(AddOperator)), Terms(std::move(Terms)) {}

  SignId getSignKind() { return SignKind; }
  Term* getTrm() { return Trm; }
  llvm::SmallVector<AddOperator*> getAddOperators() { return AddOperators; };
  llvm::SmallVector<Term*> getTerms() { return Terms; };
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class AddOperator : public AST {
 public:
  enum AddOperatorId : short {
    Plus,
    Minus
  };
 private:
  AddOperatorId AddOperatorKind;
 public:
  explicit AddOperator(AddOperatorId AddOperatorKind) : AddOperatorKind(AddOperatorKind) {}
  AddOperatorId getAddOperatorKind() { return AddOperatorKind; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Term : public AST {
  Factor* Fctor;
  llvm::SmallVector<MulOperator*> MulOperators;
  llvm::SmallVector<Factor*> Factors;
 public:
  explicit Term(
      Factor* Fctor,
      llvm::SmallVector<MulOperator*> MulOperators,
      llvm::SmallVector<Factor*> Factors)
    : Fctor(Fctor), MulOperators(std::move(MulOperators)), Factors(std::move(Factors)) {}

  Factor* getFctor() { return Fctor; };
  llvm::SmallVector<MulOperator*> getMulOperators() { return MulOperators; }
  llvm::SmallVector<Factor*> getFactors() { return Factors; } ;
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class MulOperator : public AST {
 public:
  enum MulOperatorId : short {
    Multiple,
    Divide
  };
 private:
  MulOperatorId MulOperatorKind;
 public:
  explicit MulOperator(MulOperatorId MulOperatorKind) : MulOperatorKind(MulOperatorKind) {}
  MulOperatorId getMulOperatorKind() { return MulOperatorKind; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Factor : public AST {
 public:
  virtual ~Factor() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class FunctionCall : public AST {
  Identifier* Ident;
  ExpressionList* ExpList;

 public:
  FunctionCall(Identifier* Ident, ExpressionList* ExpList) : Ident(Ident), ExpList(ExpList) {}
  Identifier* getIdent() { return Ident; }
  ExpressionList* getExpList() { return ExpList; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionList : public AST {
  llvm::SmallVector<Expression*> Expressions;
 public:
  explicit ExpressionList(llvm::SmallVector<Expression*> Expressions)
    : Expressions(std::move(Expressions)) {}
  llvm::SmallVector<Expression*> getExpressions() { return Expressions; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class GetByIndex : public AST {
  Expression* Expr;
 public:
  explicit GetByIndex(Expression* Expr) : Expr(Expr) {}

  Expression* getExpr() { return Expr; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class Identifier : public AST {
  llvm::StringRef Value;
 public:
  explicit Identifier(llvm::StringRef Value) : Value(Value) {}
  llvm::StringRef getValue() { return Value; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IntegerLiteral : public AST {
  int64_t Value;
 public:
  explicit IntegerLiteral(int64_t Value) : Value(Value) {}
  int64_t getValue() { return Value; }
  virtual void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

#endif //AST_H
