#include "Bytecode/BytecodeGenerator.h"
#include "Bytecode/BytecodeBuilder.h"
#include "Parser/AST.h"

#include <unordered_map>

class ToBytecode : public ASTVisitor {
  BytecodeBuilder Builder;
  int LabelCtr = 0;
  std::string CurWhileConditionLabel;
  std::string CurWhileAfterLabel;
  bool IsAssignment = false;
  std::unordered_map<std::string, TypeAST::TypeKind> TypeMap;
  std::string CurArrToAssign;
  bool IsAssignedArr;

  std::string newLabel() {
    return std::to_string(LabelCtr++);
  }

 public:
  std::vector<std::pair<Operation, std::vector<std::string>>> generate(AST& Ast);

  void visit(CompilationUnitAST& Node) override {
    for (auto Declaration : Node.Declarations) {
      Declaration->accept(*this);
    }
  }

  void visit(VariableDeclarationAST& Node) override {
    TypeMap[Node.Ident->Value] = Node.T->Type;
    if (Node.Expr) {
      if (Node.T->Type == TypeAST::Array) {
        CurArrToAssign = Node.Ident->Value;
        IsAssignedArr = false;
      }
      Node.Expr->accept(*this);
      if (Node.T->Type == TypeAST::Integer) {
        Builder.assign(Node.Ident->Value);
      } else if (!IsAssignedArr) {
        Builder.storeArray(Node.Ident->Value);
      }
    } else if (Node.T->Type == TypeAST::Array) {
      auto* ArrayType = dynamic_cast<ArrayTypeAST*>(Node.T);
      Builder.push(ArrayType->Size->Value);
      Builder.allocNewArray(Node.Ident->Value);
    } else {
      Builder.push("0");
      Builder.assign(Node.Ident->Value);
    }
  }

  void visit(FunctionDeclarationAST& Node) override {
    TypeMap.clear();
    LabelCtr = 0;
    std::vector<std::string> Names;
    Names.push_back(Node.Ident->Value);
    for (int i = 0; i < Node.Arguments->Idents.size(); i++) {
      if (Node.Arguments->Types[i]->Type == TypeAST::Integer) {
        Names.emplace_back("integer");
      } else {
        Names.emplace_back("array");
      }
      Names.push_back(Node.Arguments->Idents[i]->Value);
    }
    Builder.addFunction(Names);

    Node.Body->accept(*this);

    Builder.endFunction();
  }

  void visit(StatementSequenceAST& Node) override {
    for (auto& Statement : Node.Statements) {
      Statement->accept(*this);
    }
  }

  void visit(IfStatementAST& Node) override {
    Node.Condition->RHS->accept(*this);
    Node.Condition->LHS->accept(*this);
    Builder.cmp();
    auto ElseLabel = newLabel();
    auto AfterLabel = newLabel();
    switch (Node.Condition->Rel->RelKind) {
      case RelationAST::Less:Builder.jumpGe(ElseLabel);
        break;
      case RelationAST::Equal:Builder.jumpNe(ElseLabel);
        break;
      case RelationAST::NotEqual:Builder.jumpEq(ElseLabel);
        break;
      case RelationAST::LessEq:Builder.jumpGt(ElseLabel);
        break;
      case RelationAST::Greater:Builder.jumpLe(ElseLabel);
        break;
      case RelationAST::GreaterEq:Builder.jumpLt(ElseLabel);
        break;
    }

    Node.Body->accept(*this);
    Builder.addGoto(AfterLabel);

    Builder.label(ElseLabel);
    Node.ElseBody->accept(*this);

    Builder.label(AfterLabel);
  }

  void visit(WhileStatementAST& Node) override {
    auto CondLabel = newLabel();
    auto AfterLabel = newLabel();

    Builder.label(CondLabel);
    Node.Condition->RHS->accept(*this);
    Node.Condition->LHS->accept(*this);

    Builder.cmp();
    switch (Node.Condition->Rel->RelKind) {
      case RelationAST::Less:Builder.jumpGe(AfterLabel);
        break;
      case RelationAST::Equal:Builder.jumpNe(AfterLabel);
        break;
      case RelationAST::NotEqual:Builder.jumpEq(AfterLabel);
        break;
      case RelationAST::LessEq:Builder.jumpGt(AfterLabel);
        break;
      case RelationAST::Greater:Builder.jumpLe(AfterLabel);
        break;
      case RelationAST::GreaterEq:Builder.jumpLt(AfterLabel);
        break;
    }

    CurWhileConditionLabel = CondLabel;
    CurWhileAfterLabel = AfterLabel;
    Node.Body->accept(*this);
    Builder.addGoto(CondLabel);

    Builder.label(AfterLabel);
  }

  void visit(BreakStatementAST& Node) override {
    Builder.addGoto(CurWhileAfterLabel);
  }

  void visit(ContinueStatementAST& Node) override {
    Builder.addGoto(CurWhileConditionLabel);
  }

  void visit(ReturnStatementAST& Node) override {
    if (Node.Expr) {
      Node.Expr->accept(*this);
    } else {
      Builder.push("0");
    }
    Builder.addReturn();
  }

  void visit(AssignStatementAST& Node) override {
    if (Node.RHS) {
      Node.RHS->accept(*this);
      IsAssignment = true;
      Node.LHS->accept(*this);
      IsAssignment = false;
    } else {
      Node.LHS->accept(*this);
    }
  }

  void visit(PrintStatementAST& Node) override {
    Node.Expr->accept(*this);
    Builder.print();
  }

  void visit(IntegerTypeAST&) override {
  }

  void visit(ArrayTypeAST& Node) override {
  }

  void visit(ArgumentsListAST&) override {
  }

  void visit(ExpressionsListAST&) override {
  }

  void visit(ExpressionAST& Node) override {
    if (Node.RHS) {
      Node.RHS->accept(*this);
    }
    Node.LHS->accept(*this);
    if (Node.Rel) {
      Builder.cmp();
      auto FalseLabel = newLabel();
      auto AfterLabel = newLabel();
      switch (Node.Rel->RelKind) {
        case RelationAST::Less:Builder.jumpGe(FalseLabel);
          break;
        case RelationAST::Equal:Builder.jumpNe(FalseLabel);
          break;
        case RelationAST::NotEqual:Builder.jumpEq(FalseLabel);
          break;
        case RelationAST::LessEq:Builder.jumpGt(FalseLabel);
          break;
        case RelationAST::Greater:Builder.jumpLe(FalseLabel);
          break;
        case RelationAST::GreaterEq:Builder.jumpLt(FalseLabel);
          break;
      }
      Builder.push("1");
      Builder.addGoto(AfterLabel);
      Builder.label(FalseLabel);
      Builder.push("0");
      Builder.label(AfterLabel);
    }
  }

  void visit(RelationAST& Node) override {
  }

  void visit(SimpleExpressionAST& Node) override {
    std::vector<TermAST*> Terms;
    Terms.push_back(Node.Trm);
    for (int i = 0; i < Node.Terms.size(); i++) {
      Terms.push_back(Node.Terms[i]);
    }
    Terms.back()->accept(*this);
    for (int i = Terms.size() - 2; i >= 0; i--) {
      Terms[i]->accept(*this);
      switch (Node.AddOperators[i]->AddOperatorKind) {
        case AddOperatorAST::Plus:
          Builder.add();
          break;
        case AddOperatorAST::Minus:
          Builder.sub();
          break;
      }
    }
  }

  void visit(AddOperatorAST&) override {
  }

  void visit(TermAST& Node) override {
    std::vector<MulOperandAST*> Operands;
    Operands.push_back(Node.MulOperand);
    for (int i = 0; i < Node.MulOperands.size(); i++) {
      Operands.push_back(Node.MulOperands[i]);
    }
    Operands.back()->accept(*this);
    for (int i = Operands.size() - 2; i >= 0; i--) {
      Operands[i]->accept(*this);
      switch (Node.MulOperators[i]->MulOperatorKind) {
        case MulOperatorAST::Multiple:
          Builder.mul();
          break;
        case MulOperatorAST::Divide:
          Builder.div();
          break;
        case MulOperatorAST::Modulo:
          Builder.mod();
          break;
      }
    }
  }

  void visit(MulOperatorAST&) override {
  }

  void visit(MulOperandAST& Node) override {
    Node.Factor->accept(*this);
    if (Node.Operator && Node.Operator->Kind == UnaryOperatorAST::Minus) {
      Builder.push("0");
      Builder.sub();
    }
  }

  void visit(UnaryOperatorAST&) override {
  }

  void visit(IdentifierAST& Node) override {
    if (IsAssignment) {
      if (TypeMap[Node.Value] == TypeAST::Integer) {
        Builder.assign(Node.Value);
      } else {
        Builder.storeArray(Node.Value);
      }
    } else {
      if (TypeMap[Node.Value] == TypeAST::Integer) {
        Builder.load(Node.Value);
      } else {
        Builder.loadArray(Node.Value);
      }
    }
  }

  void visit(IntegerLiteralAST& Node) override {
    Builder.push(Node.Value);
  }

  void visit(ArrayInitializationAST& Node) override {
    Builder.push(std::to_string(Node.Exprs.size()));
    Builder.allocNewArray(CurArrToAssign);
    IsAssignedArr = true;
    for (int i = 0; i < Node.Exprs.size(); i++) {
      Node.Exprs[i]->accept(*this);
      Builder.push(std::to_string(i));
      Builder.assignByIndex(CurArrToAssign);
    }
  }

  void visit(GetByIndexAST& Node) override {
    Node.Index->accept(*this);
    if (IsAssignment) {
      Builder.assignByIndex(Node.Ident->Value);
    } else {
      Builder.loadByIndex(Node.Ident->Value);
    }

  }

  void visit(ExpressionFactorAST& Node) override {
    Node.Expr->accept(*this);
  }

  void visit(FunctionCallAST& Node) override {
    for (int i = Node.ExprList->Exprs.size() - 1; i >= 0; i--) {
      Node.ExprList->Exprs[i]->accept(*this);
    }
    Builder.addFunctionCall(Node.Ident->Value);
  }
};

std::vector<std::pair<Operation, std::vector<std::string>>> ToBytecode::generate(AST& Ast) {
  Ast.accept(*this);
  return Builder.build();
}

std::vector<std::pair<Operation, std::vector<std::string>>> BytecodeGenerator::generate(AST& Ast) {
  ToBytecode ToBytecodeVisitor;
  return ToBytecodeVisitor.generate(Ast);
}
