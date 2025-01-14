#ifndef AST_H
#define AST_H

#include <llvm/ADT/StringMap.h>

#include <utility>
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;

class CompilationUnitAST;
class DeclarationAST;
class VariableDeclarationAST;
class FunctionDeclarationAST;

class StatementSequenceAST;
class StatementAST;
class IfStatementAST;
class WhileStatementAST;
class ReturnStatementAST;
class AssignStatementAST;
class PrintStatementAST;

class TypeAST;
class IntegerTypeAST;
class ArrayTypeAST;
class ArgumentsListAST;
class ExpressionsListAST;

class ExpressionAST;
class RelationAST;
class SimpleExpressionAST;
class AddOperatorAST;
class TermAST;
class MulOperatorAST;
class MulOperandAST;
class UnaryOperatorAST;

class FactorAST;
class IdentifierAST;
class IntegerLiteralAST;
class ArrayInitializationAST;
class GetByIndexAST;
class ExpressionFactorAST;
class FunctionCallAST;

class ASTVisitor {
 public:
  virtual void visit(AST&) {};
  virtual void visit(CompilationUnitAST&) = 0;
  virtual void visit(DeclarationAST&) {};
  virtual void visit(VariableDeclarationAST&) = 0;
  virtual void visit(FunctionDeclarationAST&) = 0;

  virtual void visit(StatementSequenceAST&) = 0;
  virtual void visit(StatementAST&) {};
  virtual void visit(IfStatementAST&) = 0;
  virtual void visit(WhileStatementAST&) = 0;
  virtual void visit(ReturnStatementAST&) = 0;
  virtual void visit(AssignStatementAST&) = 0;
  virtual void visit(PrintStatementAST&) = 0;

  virtual void visit(TypeAST&) {};
  virtual void visit(IntegerTypeAST&) = 0;
  virtual void visit(ArrayTypeAST&) = 0;
  virtual void visit(ArgumentsListAST&) = 0;
  virtual void visit(ExpressionsListAST&) = 0;

  virtual void visit(ExpressionAST&) = 0;
  virtual void visit(RelationAST&) = 0;
  virtual void visit(SimpleExpressionAST&) = 0;
  virtual void visit(AddOperatorAST&) = 0;
  virtual void visit(TermAST&) = 0;
  virtual void visit(MulOperatorAST&) = 0;
  virtual void visit(MulOperandAST&) = 0;
  virtual void visit(UnaryOperatorAST&) = 0;

  virtual void visit(FactorAST&) {};
  virtual void visit(IdentifierAST&) = 0;
  virtual void visit(IntegerLiteralAST&) = 0;
  virtual void visit(ArrayInitializationAST&) = 0;
  virtual void visit(GetByIndexAST&) = 0;
  virtual void visit(ExpressionFactorAST&) = 0;
  virtual void visit(FunctionCallAST&) = 0;
};

class AST {
 public:
  virtual ~AST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class CompilationUnitAST : public AST {
 public:
  using DeclarationsVector = llvm::SmallVector<DeclarationAST*, 8>;
  DeclarationsVector Declarations;

  explicit CompilationUnitAST(DeclarationsVector Declarations) : Declarations(std::move(Declarations)) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class DeclarationAST : AST {
 public:
  virtual ~DeclarationAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class FunctionDeclarationAST : public DeclarationAST {
 public:
  TypeAST* ReturnType;
  IdentifierAST* Ident;
  ArgumentsListAST* Arguments;
  StatementSequenceAST* Body;

  explicit FunctionDeclarationAST(TypeAST* Type,
                                  IdentifierAST* Ident,
                                  ArgumentsListAST* Arguments,
                                  StatementSequenceAST* Body)
      : ReturnType(Type), Ident(Ident), Arguments(Arguments), Body(Body) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class StatementSequenceAST : public AST {
 public:
  std::vector<StatementAST*> Statements;

  explicit StatementSequenceAST(std::vector<StatementAST*> Statements) : Statements(std::move(Statements)) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class StatementAST : public DeclarationAST {
 public:
  virtual ~StatementAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class VariableDeclarationAST : public StatementAST {
 public:
  TypeAST* T;
  IdentifierAST* Ident;
  ExpressionAST* Expr;

  explicit VariableDeclarationAST(TypeAST* Type, IdentifierAST* Ident, ExpressionAST* Expression)
      : T(Type), Ident(Ident), Expr(Expression) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IfStatementAST : public StatementAST {
 public:
  ExpressionAST* Condition;
  StatementSequenceAST* Body;
  StatementSequenceAST* ElseBody;

  explicit IfStatementAST(ExpressionAST* Condition, StatementSequenceAST* Body, StatementSequenceAST* ElseBody)
      : Condition(Condition), Body(Body), ElseBody(ElseBody) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class WhileStatementAST : public StatementAST {
 public:
  ExpressionAST* Condition;
  StatementSequenceAST* Body;

  explicit WhileStatementAST(ExpressionAST* Condition, StatementSequenceAST* Body)
      : Condition(Condition), Body(Body) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ReturnStatementAST : public StatementAST {
 public:
  ExpressionAST* Expr;

  explicit ReturnStatementAST(ExpressionAST* Expr)
      : Expr(Expr) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class AssignStatementAST : public StatementAST {
 public:
  ExpressionAST* LHS;
  ExpressionAST* RHS;
  explicit AssignStatementAST(ExpressionAST* LHS, ExpressionAST* RHS)
      : LHS(LHS), RHS(RHS) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class PrintStatementAST : public StatementAST {
 public:
  ExpressionAST* Expr;
  explicit PrintStatementAST(ExpressionAST* Expr) : Expr(Expr) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};
class TypeAST : public AST {
 public:
  enum TypeKind {
    Integer,
    Array,
    Void
  };
  TypeKind Type;
  explicit TypeAST(enum TypeKind Type) : Type(Type) {}
  virtual ~TypeAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class VoidTypeAST : public TypeAST {
 public:
  VoidTypeAST() : TypeAST(TypeKind::Void) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IntegerTypeAST : public TypeAST {
 public:
  IntegerTypeAST() : TypeAST(TypeKind::Integer) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ArrayTypeAST : public TypeAST {
 public:
  IntegerLiteralAST* Size;
  explicit ArrayTypeAST(IntegerLiteralAST* Size)
      : Size(Size), TypeAST(TypeKind::Array) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionAST : public AST {
 public:
  SimpleExpressionAST* LHS;
  RelationAST* Rel;
  SimpleExpressionAST* RHS;

  explicit ExpressionAST(SimpleExpressionAST* LHS, RelationAST* Rel,
                         SimpleExpressionAST* RHS) : LHS(LHS), Rel(Rel), RHS(RHS) {}

  void accept(ASTVisitor& V) override {
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

  RelationId RelKind;
  explicit RelationAST(RelationId RelKind) : RelKind(RelKind) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class SimpleExpressionAST : public AST {
 public:
  TermAST* Trm;
  llvm::SmallVector<AddOperatorAST*> AddOperators;
  llvm::SmallVector<TermAST*> Terms;

  explicit SimpleExpressionAST(
      TermAST* Trm,
      llvm::SmallVector<AddOperatorAST*> AddOperator,
      llvm::SmallVector<TermAST*> Terms)
      : Trm(Trm), AddOperators(std::move(AddOperator)), Terms(std::move(Terms)) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class AddOperatorAST : public AST {
 public:
  enum AddOperatorId : short {
    Plus,
    Minus
  };
  AddOperatorId AddOperatorKind;

  explicit AddOperatorAST(AddOperatorId AddOperatorKind) : AddOperatorKind(AddOperatorKind) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class TermAST : public AST {
 public:
  MulOperandAST* MulOperand;
  llvm::SmallVector<MulOperatorAST*> MulOperators;
  llvm::SmallVector<MulOperandAST*> MulOperands;

  explicit TermAST(
      MulOperandAST* MulOperand,
      llvm::SmallVector<MulOperatorAST*> MulOperators,
      llvm::SmallVector<MulOperandAST*> MulOperands)
      : MulOperand(MulOperand), MulOperators(std::move(MulOperators)), MulOperands(std::move(MulOperands)) {}

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class MulOperatorAST : public AST {
 public:
  enum MulOperatorId : short {
    Multiple,
    Divide
  };

  MulOperatorId MulOperatorKind;
  explicit MulOperatorAST(MulOperatorId MulOperatorKind) : MulOperatorKind(MulOperatorKind) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class MulOperandAST : public AST {
 public:
  FactorAST* Factor;
  UnaryOperatorAST* Operator;

  explicit MulOperandAST(FactorAST* Factor, UnaryOperatorAST* Operator)
      : Factor(Factor), Operator(Operator) {}
      void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class UnaryOperatorAST : public AST {
 public:
  enum UnaryOperatorKind : short {
    Minus,
    Plus
  };
  UnaryOperatorKind Kind;

  explicit UnaryOperatorAST(UnaryOperatorKind Kind) : Kind(Kind) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class FactorAST : public AST {
 public:
  virtual ~FactorAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class GetByIndexAST : public FactorAST {
 public:
  IdentifierAST* Ident;
  ExpressionAST* Index;

  explicit GetByIndexAST(IdentifierAST* Ident, ExpressionAST* Expr)
      : Ident(Ident), Index(Expr) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IdentifierAST : public FactorAST {
 public:
  llvm::StringRef Value;
  explicit IdentifierAST(llvm::StringRef Value) : Value(Value) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IntegerLiteralAST : public FactorAST {
 public:
  llvm::StringRef Value;
  explicit IntegerLiteralAST(llvm::StringRef Value) : Value(Value) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ArrayInitializationAST : public FactorAST {
 public:
  llvm::SmallVector<ExpressionAST*> Exprs;

  explicit ArrayInitializationAST(llvm::SmallVector<ExpressionAST*> Exprs) : Exprs(std::move(Exprs)) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionFactorAST : public FactorAST {
 public:
  ExpressionAST* Expr;

  explicit ExpressionFactorAST(ExpressionAST* Expr) : Expr(Expr) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionsListAST : public AST {
 public:
  llvm::SmallVector<ExpressionAST*> Exprs;

  explicit ExpressionsListAST(llvm::SmallVector<ExpressionAST*> Exprs) : Exprs(std::move(Exprs)) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ArgumentsListAST : public AST {
 public:
  llvm::SmallVector<IdentifierAST*> Idents;
  llvm::SmallVector<TypeAST*> Types;

  explicit ArgumentsListAST(llvm::SmallVector<IdentifierAST*> Idents, llvm::SmallVector<TypeAST*> Types)
      : Idents(std::move(Idents)), Types(std::move(Types)) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class FunctionCallAST : public FactorAST {
 public:
  IdentifierAST* Ident;
  ExpressionsListAST* ExprList;

  explicit FunctionCallAST(IdentifierAST* Ident, ExpressionsListAST* ExprList)
      : Ident(Ident), ExprList(ExprList) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

#endif //AST_H
