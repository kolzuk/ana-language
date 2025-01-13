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
  virtual void visit(StatementAST&) = 0;
  virtual void visit(IfStatementAST&) = 0;
  virtual void visit(WhileStatementAST&) = 0;
  virtual void visit(ReturnStatementAST&) = 0;
  virtual void visit(AssignStatementAST&) = 0;

  virtual void visit(TypeAST&) = 0;
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
  using DeclarationsVector = llvm::SmallVector<DeclarationAST*, 8>;
  DeclarationsVector Declarations;
 public:
  DeclarationsVector getDeclarations() { return Declarations; }
  explicit CompilationUnitAST(DeclarationsVector Declarations) : Declarations(std::move(Declarations)) {}
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class DeclarationAST {
 public:
  virtual ~DeclarationAST() = default;
  virtual void accept(ASTVisitor& V) = 0;
};

class VariableDeclarationAST : public DeclarationAST {
  TypeAST* T;
  IdentifierAST* Ident;
  ExpressionAST* Expr;
 public:
  explicit VariableDeclarationAST(TypeAST* Type, IdentifierAST* Ident, ExpressionAST* Expression)
      : T(Type), Ident(Ident), Expr(Expression) {}

  TypeAST* getType() { return T; }
  IdentifierAST* getIdentifier() { return Ident; };
  ExpressionAST* getExpression() { return Expr; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class FunctionDeclarationAST : public DeclarationAST {
  TypeAST* ReturnType;
  IdentifierAST* Ident;
  ArgumentsListAST* Arguments;
  StatementSequenceAST* Body;
 public:
  explicit FunctionDeclarationAST(TypeAST* Type,
                                  IdentifierAST* Ident,
                                  ArgumentsListAST* Arguments,
                                  StatementSequenceAST* Body)
      : ReturnType(Type), Ident(Ident), Arguments(Arguments), Body(Body) {}

  TypeAST* getType() { return ReturnType; }
  IdentifierAST* getIdentifier() { return Ident; };
  ArgumentsListAST* getArgumentsList() { return Arguments; }
  StatementSequenceAST* getBody() { return Body; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class StatementSequenceAST : public AST {
  std::vector<StatementAST*> Statements;
 public:
  explicit StatementSequenceAST(std::vector<StatementAST*> Statements) : Statements(std::move(Statements)) {}

  std::vector<StatementAST*> getStatements() { return Statements; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class StatementAST : public AST {};

class IfStatementAST : public StatementAST {
  ExpressionAST* Condition;
  StatementSequenceAST* Body;
  StatementSequenceAST* ElseBody;
 public:
  explicit IfStatementAST(ExpressionAST* Condition, StatementSequenceAST* Body, StatementSequenceAST* ElseBody)
      : Condition(Condition), Body(Body), ElseBody(ElseBody) {}

  ExpressionAST* getCondition() { return Condition; }
  StatementSequenceAST* getBody() { return Body; }
  StatementSequenceAST* getElseBody() { return ElseBody; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class WhileStatementAST : public StatementAST {
  ExpressionAST* Condition;
  StatementSequenceAST* Body;
 public:
  explicit WhileStatementAST(ExpressionAST* Condition, StatementSequenceAST* Body)
      : Condition(Condition), Body(Body) {}

  ExpressionAST* getCondition() { return Condition; }
  StatementSequenceAST* getBody() { return Body; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ReturnStatementAST : public StatementAST {
  ExpressionAST* Expr;
 public:
  explicit ReturnStatementAST(ExpressionAST* Expr)
      : Expr(Expr) {}

  ExpressionAST* getExpr() { return Expr; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class AssignStatementAST : public StatementAST {
  ExpressionAST* LHS;
  ExpressionAST* RHS;
 public:
  explicit AssignStatementAST(ExpressionAST* LHS, ExpressionAST* RHS)
      : LHS(LHS), RHS(RHS) {}

  ExpressionAST* getLHS() { return LHS; }
  ExpressionAST* getRHS() { return RHS; }

  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class TypeAST : public AST {};

class VoidTypeAST : public TypeAST {
 public:
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IntegerTypeAST : public TypeAST {
 public:
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ArrayTypeAST : public TypeAST {
  ExpressionAST* Size;
 public:
  explicit ArrayTypeAST(ExpressionAST* Size)
      : Size(Size) {}
  ExpressionAST* getSize() { return Size; }
  void accept(ASTVisitor& V) override {
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
 private:
  RelationId RelKind;
 public:
  explicit RelationAST(RelationId RelKind) : RelKind(RelKind) {}
  RelationId getRelKind() { return RelKind; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class SimpleExpressionAST : public AST {
 private:
  TermAST* Trm;
  llvm::SmallVector<AddOperatorAST*> AddOperators;
  llvm::SmallVector<TermAST*> Terms;

 public:
  explicit SimpleExpressionAST(
      TermAST* Trm,
      llvm::SmallVector<AddOperatorAST*> AddOperator,
      llvm::SmallVector<TermAST*> Terms)
      : Trm(Trm), AddOperators(std::move(AddOperator)), Terms(std::move(Terms)) {}

  TermAST* getTrm() { return Trm; }
  llvm::SmallVector<AddOperatorAST*> getAddOperators() { return AddOperators; };
  llvm::SmallVector<TermAST*> getTerms() { return Terms; };
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
 private:
  AddOperatorId AddOperatorKind;
 public:
  explicit AddOperatorAST(AddOperatorId AddOperatorKind) : AddOperatorKind(AddOperatorKind) {}
  AddOperatorId getAddOperatorKind() { return AddOperatorKind; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class TermAST : public AST {
  MulOperandAST* MulOperand;
  llvm::SmallVector<MulOperatorAST*> MulOperators;
  llvm::SmallVector<MulOperandAST*> MulOperands;
 public:
  explicit TermAST(
      MulOperandAST* MulOperand,
      llvm::SmallVector<MulOperatorAST*> MulOperators,
      llvm::SmallVector<MulOperandAST*> MulOperands)
      : MulOperand(MulOperand), MulOperators(std::move(MulOperators)), MulOperands(std::move(MulOperands)) {}

  MulOperandAST* getMulOperand() { return MulOperand; };
  llvm::SmallVector<MulOperatorAST*> getMulOperators() { return MulOperators; }
  llvm::SmallVector<MulOperandAST*> getMulOperands() { return MulOperands; };
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
 private:
  MulOperatorId MulOperatorKind;
 public:
  explicit MulOperatorAST(MulOperatorId MulOperatorKind) : MulOperatorKind(MulOperatorKind) {}
  MulOperatorId getMulOperatorKind() { return MulOperatorKind; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class MulOperandAST : public AST {
  FactorAST* Factor;
  UnaryOperatorAST* Operator;
 public:
  explicit MulOperandAST(FactorAST* Factor, UnaryOperatorAST* Operator)
      : Factor(Factor), Operator(Operator) {}

  FactorAST* getFactor() { return Factor; };
  UnaryOperatorAST* getOperator() { return Operator; }
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

 private:
  UnaryOperatorKind Kind;
 public:
  explicit UnaryOperatorAST(UnaryOperatorKind Kind) : Kind(Kind) {}
  UnaryOperatorKind getUnaryOperatorKind() { return Kind; }
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
  IdentifierAST* BaseIdentifier;
  ExpressionAST* Expr;
 public:
  explicit GetByIndexAST(IdentifierAST* BaseIdentifier, ExpressionAST* Expr)
      : BaseIdentifier(BaseIdentifier), Expr(Expr) {}

  IdentifierAST* getIdentifier() { return BaseIdentifier; }
  ExpressionAST* getExpr() { return Expr; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IdentifierAST : public FactorAST {
  llvm::StringRef Value;
 public:
  explicit IdentifierAST(llvm::StringRef Value) : Value(Value) {}
  llvm::StringRef getValue() { return Value; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class IntegerLiteralAST : public FactorAST {
  llvm::StringRef Value;
 public:
  explicit IntegerLiteralAST(llvm::StringRef Value) : Value(Value) {}
  llvm::StringRef getValue() { return Value; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ArrayInitializationAST : public FactorAST {
  ExpressionAST* Expr;
 public:
  explicit ArrayInitializationAST(ExpressionAST* Expr) : Expr(Expr) {}

  ExpressionAST* getExpr() { return Expr; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionFactorAST : public FactorAST {
  ExpressionAST* Expr;
 public:
  explicit ExpressionFactorAST(ExpressionAST* Expr) : Expr(Expr) {}

  ExpressionAST* getExpr() { return Expr; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ExpressionsListAST : public AST {
  llvm::SmallVector<ExpressionAST*> Exprs;
 public:
  explicit ExpressionsListAST(llvm::SmallVector<ExpressionAST*> Exprs) : Exprs(std::move(Exprs)) {}

  llvm::SmallVector<ExpressionAST*> getExprs() { return Exprs; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

class ArgumentsListAST : public AST {
  llvm::SmallVector<IdentifierAST*> Idents;
  llvm::SmallVector<TypeAST*> Types;
 public:
  explicit ArgumentsListAST(llvm::SmallVector<IdentifierAST*> Idents, llvm::SmallVector<TypeAST*> Types)
      : Idents(std::move(Idents)), Types(std::move(Types)) {}

  llvm::SmallVector<IdentifierAST*> getIdents() { return Idents; }
  llvm::SmallVector<TypeAST*> getTypes() { return Types; }
  void accept(ASTVisitor& V) override {
    V.visit(*this);
  }
};

#endif //AST_H
