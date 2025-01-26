#include "Bytecode/BytecodeGenerator.h"
#include "Bytecode/BytecodeBuilder.h"
#include "Parser/AST.h"

#include <cstdint>
#include <unordered_map>

class ToBytecode : public ASTVisitor {
  BytecodeBuilder Builder;
  int LabelCtr = 0;
  std::string CurWhileConditionLabel;
  std::string CurWhileAfterLabel;
  bool IsAssignment = false;
  std::unordered_map<std::string, TypeAST::TypeKind> TypeMap;

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
      Node.Expr->accept(*this);

      if (Node.T->Type == TypeAST::Array)
        Builder.arrayStore(Node.Ident->Value);
      if (Node.T->Type == TypeAST::Integer)
        Builder.integerStore(Node.Ident->Value);
    } else {
      Builder.push("0");
      Builder.integerStore(Node.Ident->Value);
    }
  }

  void visit(FunctionDeclarationAST& Node) override {
    TypeMap.clear();
    LabelCtr = 0;
    std::vector<std::string> Names;
    Names.push_back(Node.Ident->Value);
    for (int i = 0; i < Node.Arguments->Idents.size(); i++) {
      if (Node.Arguments->Types[i]->Type == TypeAST::Integer) {
        TypeMap[Node.Arguments->Idents[i]->Value] = TypeAST::Integer;
        Names.emplace_back("integer");
      } else {
        TypeMap[Node.Arguments->Idents[i]->Value] = TypeAST::Array;
        Names.emplace_back("array");
      }
      Names.push_back(Node.Arguments->Idents[i]->Value);
    }
    Builder.funBegin(Names);

    Node.Body->accept(*this);

    if (Builder.getLastCommand().first != RETURN) {
      Builder.push("0");
      Builder.addReturn();
    }

    Builder.funEnd();
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
    auto Label = newLabel();
    switch (Node.Condition->Rel->RelKind) {
      case RelationAST::Less:Builder.jumpGe(Label);
        break;
      case RelationAST::Equal:Builder.jumpNe(Label);
        break;
      case RelationAST::NotEqual:Builder.jumpEq(Label);
        break;
      case RelationAST::LessEq:Builder.jumpGt(Label);
        break;
      case RelationAST::Greater:Builder.jumpLe(Label);
        break;
      case RelationAST::GreaterEq:Builder.jumpLt(Label);
        break;
    }

    Node.Body->accept(*this);

    if (Node.ElseBody->Statements.empty()) {
      Builder.jump(Label);
      Builder.label(Label);
      return;
    }
    auto AfterLabel = newLabel();
    Builder.jump(AfterLabel);
    Builder.label(Label);
    Node.ElseBody->accept(*this);
    Builder.label(AfterLabel);
  }

  void visit(ForStatementAST& Node) override {
    auto CondLabel = newLabel();
    auto AfterLabel = newLabel();

    Node.Initialization->accept(*this);
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

    auto Temp1 = CurWhileConditionLabel;
    auto Temp2 = CurWhileAfterLabel;
    CurWhileConditionLabel = CondLabel;
    CurWhileAfterLabel = AfterLabel;
    Node.Body->accept(*this);
    Node.Update->accept(*this);
    Builder.jump(CondLabel);
    Builder.label(AfterLabel);

    CurWhileConditionLabel = Temp1;
    CurWhileAfterLabel = Temp2;
  }


  void visit(WhileStatementAST& Node) override {
    std::string CondLabel;
    auto AfterLabel = newLabel();

    if (Builder.getLastCommand().first == LABEL) {
      CondLabel = Builder.getLastCommand().second[0];
    } else {
      CondLabel = newLabel();
      Builder.label(CondLabel);
    }

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

    auto Temp1 = CurWhileConditionLabel;
    auto Temp2 = CurWhileAfterLabel;
    CurWhileConditionLabel = CondLabel;
    CurWhileAfterLabel = AfterLabel;
    Node.Body->accept(*this);
    Builder.jump(CondLabel);
    Builder.label(AfterLabel);
    CurWhileConditionLabel = Temp1;
    CurWhileAfterLabel = Temp2;
  }

  void visit(BreakStatementAST& Node) override {
    Builder.jump(CurWhileAfterLabel);
  }

  void visit(ContinueStatementAST& Node) override {
    Builder.jump(CurWhileConditionLabel);
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
      Builder.jump(AfterLabel);
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
    for (auto Term : Node.Terms)
      Terms.push_back(Term);

    Terms[0]->accept(*this);
    for (int64_t i = 1; i < Terms.size(); ++i) {
      Terms[i]->accept(*this);
      switch (Node.AddOperators[i - 1]->AddOperatorKind) {
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
    for (auto MulOperand : Node.MulOperands)
      Operands.push_back(MulOperand);

    Operands[0]->accept(*this);
    for (int64_t i = 1; i < Operands.size(); i++) {
      Operands[i]->accept(*this);
      switch (Node.MulOperators[i - 1]->MulOperatorKind) {
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
        Builder.integerStore(Node.Value);
      } else {
        Builder.arrayStore(Node.Value);
      }
    } else {
      if (TypeMap[Node.Value] == TypeAST::Integer) {
        Builder.integerLoad(Node.Value);
      } else {
        Builder.arrayLoad(Node.Value);
      }
    }
  }

  void visit(IntegerLiteralAST& Node) override {
    Builder.push(Node.Value);
  }

  void visit(ArrayInitializationAST& Node) override {
    Node.Expr->accept(*this);
    Builder.newArray();
  }

  void visit(GetByIndexAST& Node) override {
    bool Tmp = IsAssignment;
    IsAssignment = false;
    Node.Index->accept(*this);
    IsAssignment = Tmp;
    if (IsAssignment) {
      Builder.storeInIndex(Node.Ident->Value);
    } else {
      Builder.loadFromIndex(Node.Ident->Value);
    }
  }

  void visit(ExpressionFactorAST& Node) override {
    Node.Expr->accept(*this);
  }

  void visit(FunctionCallAST& Node) override {
    for (int64_t i = Node.ExprList->Exprs.size() - 1; i >= 0; i--)
      Node.ExprList->Exprs[i]->accept(*this);

    Builder.funCall(Node.Ident->Value);
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