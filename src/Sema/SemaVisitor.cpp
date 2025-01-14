#include <llvm/Support/raw_ostream.h>

#include "Sema/Sema.h"
#include "Parser/AST.h"
#include "llvm/ADT/StringSet.h"

void SemaVisitor::error(SemaVisitor::ErrorType ET, llvm::StringRef V) {
  llvm::errs() << "Variable " << V << " "
             << (ET == Twice ? "already" : "not")
             << " declared\n";
  HasError = true;
}

bool SemaVisitor::hasError() { return HasError; }

void SemaVisitor::visit(AST& Node) {}; // Abstract

void SemaVisitor::visit(CompilationUnitAST& Node) {
  for (auto* Decl : Node.Declarations) {
    Decl->accept(*this);
  }
};

 void SemaVisitor::visit(DeclarationAST& Node) {}; // Abstract

void SemaVisitor::visit(VariableDeclarationAST& Node) {
  llvm::StringRef Name = Node.Ident->Value;
  if (!Scope.insert(Name).second) {
    error(Twice, Name);
  }
  if (Node.Expr) {
    Node.Expr->accept(*this);
  }
  Node.T->accept(*this);
};

void SemaVisitor::visit(FunctionDeclarationAST& Node) {
  llvm::StringRef Name = Node.Ident->Value;
  if (!Scope.insert(Name).second) {
    error(Twice, Name);
  }
  Node.Arguments->accept(*this);
  if (Node.ReturnType) {
    Node.ReturnType->accept(*this);
  }
  Node.Body->accept(*this);
}

void SemaVisitor::visit(StatementSequenceAST& Node) {
  for (auto* Stmt : Node.Statements) {
    Stmt->accept(*this);
  }
};

void SemaVisitor::visit(StatementAST& Node) {}; // Abstract

void SemaVisitor::visit(IfStatementAST& Node) {
  Node.Condition->accept(*this);
  Node.Body->accept(*this);
  if (Node.ElseBody) {
    Node.ElseBody->accept(*this);
  }
};

void SemaVisitor::visit(WhileStatementAST& Node) {
  Node.Condition->accept(*this);
  Node.Body->accept(*this);
};

void SemaVisitor::visit(ReturnStatementAST& Node) {
  if (Node.Expr) {
    Node.Expr->accept(*this);
  }
};

void SemaVisitor::visit(AssignStatementAST& Node) {
  Node.LHS->accept(*this);
  if (Node.RHS) {
    Node.RHS->accept(*this);
  }
}

void SemaVisitor::visit(TypeAST& Node) {}; // Abstract

void SemaVisitor::visit(IntegerTypeAST& Node) {}; // Abstract

void SemaVisitor::visit(ArrayTypeAST& Node) {
  Node.Size->accept(*this);
};

void SemaVisitor::visit(ArgumentsListAST& Node) {
  for (size_t i = 0; i < Node.Idents.size(); ++i) {
    llvm::StringRef Name = Node.Idents[i]->Value;
    if (!Scope.insert(Name).second) {
      error(Twice, Name);
    }
    Node.Types[i]->accept(*this);
  }
};

void SemaVisitor::visit(ExpressionsListAST& Node) {
  for (auto* Expr : Node.Exprs) {
    Expr->accept(*this);
  }
}

void SemaVisitor::visit(ExpressionAST& Node) {
  Node.LHS->accept(*this);
  if (Node.Rel) {
    Node.Rel->accept(*this);
    Node.RHS->accept(*this);
  }
};

void SemaVisitor::visit(RelationAST& Node) {
  // TODO
};

void SemaVisitor::visit(SimpleExpressionAST& Node) {
  Node.Trm->accept(*this);
  for (auto* Op : Node.AddOperators) {
    Op->accept(*this);
  }
  for (auto* Term : Node.Terms) {
    Term->accept(*this);
  }
};

void SemaVisitor::visit(AddOperatorAST& Node) {
  // TODO
//  if (Node.getLeft())
//    Node.getLeft()->accept(*this);
//  else
//    HasError = true;
//  if (Node.getRight())
//    Node.getRight()->accept(*this);
//  else
//    HasError = true;
};

void SemaVisitor::visit(TermAST& Node) {
  Node.MulOperand->accept(*this);
  for (auto* Op : Node.MulOperators) {
    Op->accept(*this);
  }
  for (auto* Operand : Node.MulOperands) {
    Operand->accept(*this);
  }
};

void SemaVisitor::visit(MulOperatorAST& Node) {
  // TODO
};

void SemaVisitor::visit(MulOperandAST& Node) {
  if (Node.Operator) {
    Node.Operator->accept(*this);
  }
  Node.Factor->accept(*this);
};

void SemaVisitor::visit(UnaryOperatorAST& Node) {
  // TODO
}

void SemaVisitor::visit(FactorAST& Node) { // Abstract
//  if (Node.getKind() == Factor::Ident) {
//    if (Scope.find(Node.getVal()) == Scope.end())
//      error(Not, Node.getVal());
//  }
};

void SemaVisitor::visit(IdentifierAST& Node) {
  if (Scope.find(Node.Value) == Scope.end()) {
    error(Not, Node.Value);
  }
};

void SemaVisitor::visit(IntegerLiteralAST& Node) {}; // Always correct

void SemaVisitor::visit(ArrayInitializationAST& Node) {
  for (auto* Expr : Node.Exprs) {
    Expr->accept(*this);
  }
};

void SemaVisitor::visit(GetByIndexAST& Node) {
  Node.Ident->accept(*this);
  Node.Expr->accept(*this);
};

void SemaVisitor::visit(ExpressionFactorAST& Node) {
  Node.Expr->accept(*this);
};

void SemaVisitor::visit(FunctionCallAST& Node) {
  if (Scope.find(Node.Ident->Value) == Scope.end()) {
    error(Not, Node.Ident->Value);
  }
  Node.ExprList->accept(*this);
};
