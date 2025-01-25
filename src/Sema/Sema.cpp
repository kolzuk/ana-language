//#include "Sema/Scope.h"
//#include "Sema/Sema.h"
//#include "Sema/SemaVisitor.h"
//#include "Parser/AST.h"
//
//#include <string>
//#include <iostream>
//
//Decl::Decl(std::string name, TypeAST* type, bool isInitialized, ArgumentsListAST* arguments) {
//  this->Name = name;
//  this->Tp = type;
//  this->IsInitialized = isInitialized;
//  this->Arguments = arguments;
//}
//
//bool Scope::insert(Decl* Declaration) {
//  return ScopeMembers.insert(std::pair<std::string, Decl*>(Declaration->Name, Declaration)).second;
//}
//
//Decl* Scope::lookup(std::string Name) {
//  Scope* CurrentScp = this;
//  while (CurrentScp) {
//    auto Iter = CurrentScp->ScopeMembers.find(Name);
//    if (Iter != CurrentScp->ScopeMembers.end())
//      return Iter->second;
//    CurrentScp = CurrentScp->getParent();
//  }
//  return nullptr;
//}
//
//Scope* Scope::getParent() { return Parent; }
//std::string Scope::getName() { return Name;}
//
//void SemaVisitor::error(const std::string& Message) {
//  std::cerr << Message << "\n";
//  HasError = true;
//}
//
//std::string SemaVisitor::typeToString(const TypeAST::TypeKind Type) {
//  switch (Type) {
//    case TypeAST::TypeKind::Integer:   return "integer";
//    case TypeAST::TypeKind::Array: return "array";
//    default:    return "void";
//  }
//}
//
//std::string SemaVisitor::generateAlreadyMessage(const std::string& Name) {
//  std::string Message;
//  Message.append("Entity ");
//  Message.append(Name);
//  Message.append(" is already declared");
//  return Message;
//}
//
//std::string SemaVisitor::generateNotDeclaredMessage(const std::string& Name) {
//  std::string Message;
//  Message.append("Entity ");
//  Message.append(Name);
//  Message.append(" not declared");
//  return Message;
//}
//
//std::string SemaVisitor::generateNotMatchTypeMessage(const std::string& Name, TypeAST::TypeKind Type) {
//  std::string Message;
//  Message.append("Entity ");
//  Message.append(Name);
//  Message.append(" must be assigned with type ");
//  Message.append(typeToString(Type));
//  return Message;
//}
//
//std::string SemaVisitor::generateNotMatchSizeMessage(const std::string& Name, size_t Size) {
//  std::string Message;
//  Message.append("Array variable ");
//  Message.append(Name);
//  Message.append(" must be assigned with array of size ");
//  Message.append(std::to_string(Size));
//  return Message;
//}
//
//std::string SemaVisitor::generateWrongArgumentsAmountMessage(const std::string& Name, size_t Size) {
//  std::string Message;
//  Message.append("Function ");
//  Message.append(Name);
//  Message.append(" needs exactly ");
//  Message.append(std::to_string(Size));
//  Message.append(" arguments");
//  return Message;
//}
//
//std::string SemaVisitor::generateWrongGetByIndexMessage(const std::string& Name) {
//  std::string Message;
//  Message.append("Entity ");
//  Message.append(Name);
//  Message.append(" must be array to be indexed");
//  return Message;
//}
//
//std::string SemaVisitor::generateNotIntegerIndexMessage() {
//  std::string Message;
//  Message.append("Index for array must be integer");
//  return Message;
//}
//
//std::string SemaVisitor::generateNotIntegerExpressionMessage() {
//  std::string Message;
//  Message.append("Such expression must be integer");
//  return Message;
//}
//
//bool SemaVisitor::hasError() { return HasError; }
//
//void SemaVisitor::visit(AST& Node) {}; // Abstract
//
//void SemaVisitor::visit(CompilationUnitAST& Node) {
//  if (HasError) return;
//  Scp = new Scope("global", Scp);
//  for (auto* Decl : Node.Declarations) {
//    Decl->accept(*this);
//    if (HasError) return;
//  }
//  delete Scp;
//};
//
//void SemaVisitor::visit(DeclarationAST& Node) {}; // Abstract
//
//void SemaVisitor::visit(VariableDeclarationAST& Node) {
//  if (HasError) return;
//  std::string Name = Node.Ident->Value;
//  if (Scp->lookup(Name)) {
//    error(generateAlreadyMessage(Name));
//  }
//
//  Node.T->accept(*this);
//  if (HasError) return;
//  TypeAST* Type = Node.T;
//
//  if (!Node.Expr) {
//    Scp->insert(new Decl(Name, Type, false, nullptr));
//  }
//  else {
//    TypeAST::TypeKind VarType = LastType;
//    size_t VarSize = LastArraySize;
//    Node.Expr->accept(*this);
//    if (HasError) return;
//    if (VarType != LastType) {
//      error(generateNotMatchTypeMessage(Name, VarType));
//      return;
//    }
//    if (VarType == TypeAST::TypeKind::Array and VarSize != LastArraySize) {
//      error(generateNotMatchSizeMessage(Name, VarSize));
//    }
//    Scp->insert(new Decl(Name, Type, true, nullptr));
//  }
//};
//
//void SemaVisitor::visit(FunctionDeclarationAST& Node) {
//  if (HasError) return;
//  if (HasError) return;
//  std::string Name = Node.Ident->Value;
//  if (Scp->lookup(Name)) {
//    error(generateAlreadyMessage(Name));
//  }
//
//  if (!Node.ReturnType) {
//    HasError = true;
//    return;
//  }
//
//  Node.ReturnType->accept(*this);
//  if (HasError) return;
//  Scp->insert(new Decl(Name, Node.ReturnType, true, nullptr));
//  Decl* Fun = Scp->lookup(Name);
//  Scope* ParentScope = Scp;
//  Scp = new Scope(Name, ParentScope);
//  Node.Arguments->accept(*this);
//  Fun->Arguments = Node.Arguments;
//  Node.Body->accept(*this);
//  delete Scp;
//  Scp = ParentScope;
//}
//
//void SemaVisitor::visit(StatementSequenceAST& Node) {
//  for (auto* Stmt : Node.Statements) {
//    if (HasError) return;
//    Stmt->accept(*this);
//  }
//};
//
//void SemaVisitor::visit(StatementAST& Node) {}; // Abstract
//
//void SemaVisitor::visit(IfStatementAST& Node) {
//  if (HasError) return;
//  Node.Condition->accept(*this);
//  if (LastType != TypeAST::TypeKind::Integer) {
//    error(generateNotIntegerExpressionMessage());
//  };
//  Scope* ParentScope = Scp;
//  Scp = new Scope("if", ParentScope);
//  Node.Body->accept(*this);
//  if (HasError) return;
//  delete Scp;
//  Scp = ParentScope;
//  if (Node.ElseBody) {
//    ParentScope = Scp;
//    Scp = new Scope("else", ParentScope);
//    Node.ElseBody->accept(*this);
//    delete Scp;
//    Scp = ParentScope;
//  }
//};
//
//void SemaVisitor::visit(WhileStatementAST& Node) {
//  if (HasError) return;
//  Node.Condition->accept(*this);
//  if (LastType != TypeAST::TypeKind::Integer) {
//    error(generateNotIntegerExpressionMessage());
//  };
//  Scope* ParentScope = Scp;
//  Scp = new Scope("if", ParentScope);
//  if (HasError) return;
//  Node.Body->accept(*this);
//  delete Scp;
//  Scp = ParentScope;
//};
//
//void SemaVisitor::visit(ReturnStatementAST& Node) {
//  if (HasError) return;
//  if (Node.Expr) {
//    Node.Expr->accept(*this);
//  }
//};
//
//void SemaVisitor::visit(AssignStatementAST& Node) {
//  if (HasError) return;
//  Node.LHS->accept(*this);
//  if (Node.RHS) {
//    Node.RHS->accept(*this);
//  }
//}
//
//void SemaVisitor::visit(PrintStatementAST& Node) {
//  if (HasError) return;
//  Node.Expr->accept(*this);
//}
//
//void SemaVisitor::visit(BreakStatementAST& Node) {}
//
//void SemaVisitor::visit(ContinueStatementAST& Node) {}
//
//void SemaVisitor::visit(TypeAST& Node) {}; // Abstract
//
//void SemaVisitor::visit(IntegerTypeAST& Node) {
//  LastType = Node.Type;
//};
//
//void SemaVisitor::visit(ArrayTypeAST& Node) {
//  LastType = Node.Type;
//  LastArraySize = std::stoi(Node.Size->Value);
//};
//
//void SemaVisitor::visit(ArgumentsListAST& Node) {
//  for (size_t i = 0; i < Node.Idents.size(); ++i) {
//    if (HasError) return;
//    std::string Name = Node.Idents[i]->Value;
//    if (Scp->lookup(Name)) {
//      error(generateAlreadyMessage(Name));
//    }
//    if (HasError) return;
//    Node.Types[i]->accept(*this);
//    if (HasError) return;
//    TypeAST* Type = Node.Types[i];
//    Scp->insert(new Decl(Name, Type, true, nullptr));
//  }
//};
//
//void SemaVisitor::visit(ExpressionsListAST& Node) {
//  Decl* Func = Scp->lookup(LastFunctionName);
//  if (!Func->Arguments) {
//    HasError = true;
//    return;
//  }
//  if (Node.Exprs.size() != Func->Arguments->Types.size()) {
//    error(generateWrongArgumentsAmountMessage(Func->Name, Func->Arguments->Types.size()));
//  }
//  size_t i = 0;
//  for (auto* Expr : Node.Exprs) {
//    Func->Arguments->Types[i]->accept(*this);
//    TypeAST::TypeKind VarType = LastType;
//    size_t VarSize = LastArraySize;
//    std::string Name = Func->Arguments->Idents[i]->Value;
//
//    Expr->accept(*this);
//
//    if (HasError) return;
//    if (VarType != LastType) {
//      error(generateNotMatchTypeMessage(Name, VarType));
//      return;
//    }
//    if (VarType == TypeAST::TypeKind::Array and VarSize != LastArraySize) {
//      error(generateNotMatchSizeMessage(Name, VarSize));
//    }
//    ++i;
//  }
//}
//
//void SemaVisitor::visit(ExpressionAST& Node) {
//  Node.LHS->accept(*this);
//  if (Node.Rel) {
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    };
//    Node.RHS->accept(*this);
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    };
//  }
//};
//
//void SemaVisitor::visit(RelationAST& Node) {}; // Empty
//
//void SemaVisitor::visit(SimpleExpressionAST& Node) {
//  Node.Trm->accept(*this);
//  for (auto* Operator : Node.AddOperators) {
//    Operator->accept(*this);
//  }
//  if (!Node.Terms.empty()) {
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    }
//  }
//  for (auto* Operand : Node.Terms) {
//    Operand->accept(*this);
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    };
//    if (HasError) return;
//  }
//};
//
//void SemaVisitor::visit(AddOperatorAST& Node) {}; // Empty
//
//void SemaVisitor::visit(TermAST& Node) {
//  Node.MulOperand->accept(*this);
//  for (auto* Operator : Node.MulOperators) {
//    Operator->accept(*this);
//  }
//  if (!Node.MulOperands.empty()) {
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    }
//  }
//  if (HasError) return;
//  for (auto* Operand : Node.MulOperands) {
//    Operand->accept(*this);
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    };
//    if (HasError) return;
//  }
//};
//
//void SemaVisitor::visit(MulOperatorAST& Node) {}; // Empty
//
//void SemaVisitor::visit(MulOperandAST& Node) {
//  if (HasError) return;
//  Node.Factor->accept(*this);
//  if (Node.Operator) {
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    };
//  }
//};
//
//void SemaVisitor::visit(UnaryOperatorAST& Node) {} // Empty
//
//void SemaVisitor::visit(FactorAST& Node) {}; // Abstract
//
//
//void SemaVisitor::visit(IdentifierAST& Node) {
//  if (HasError) return;
//  Decl* Declaration = Scp->lookup(Node.Value);
//  if (!Declaration) {
//    error(generateNotDeclaredMessage(Node.Value));
//  }
//  Declaration->Tp->accept(*this);
//};
//
//void SemaVisitor::visit(IntegerLiteralAST& Node) {
//  LastArraySize = std::stoi(Node.Value);
//  LastType = TypeAST::TypeKind::Integer;
//};
//
//void SemaVisitor::visit(ArrayInitializationAST& Node) {
//  for (auto* Expr : Node.Exprs) {
//    if (HasError) return;
//    Expr->accept(*this);
//    if (LastType != TypeAST::TypeKind::Integer) {
//      error(generateNotIntegerExpressionMessage());
//    };
//    if (HasError) return;
//  }
//  LastArraySize = Node.Exprs.size();
//  LastType = TypeAST::TypeKind::Array;
//};
//
//void SemaVisitor::visit(GetByIndexAST& Node) {
//  Node.Ident->accept(*this);
//  if (HasError) return;
//  if (LastType != TypeAST::TypeKind::Array) {
//    error(generateWrongGetByIndexMessage(Node.Ident->Value));
//  }
//  if (HasError) return;
//  Node.Index->accept(*this);
//  if (LastType != TypeAST::TypeKind::Integer) {
//    error(generateNotIntegerIndexMessage());
//  };
//  LastType = TypeAST::TypeKind::Integer;
//};
//
//void SemaVisitor::visit(ExpressionFactorAST& Node) {
//  Node.Expr->accept(*this);
//};
//
//void SemaVisitor::visit(FunctionCallAST& Node) {
//  std::string Name = Node.Ident->Value;
//  Decl* Func = Scp->lookup(Name);
//  if (!Func) {
//    error(generateNotDeclaredMessage(Name));
//  }
//  LastFunctionName = Name;
//  if (HasError) return;
//  Node.ExprList->accept(*this);
//  Func->Tp->accept(*this); // to store last type
//};
//
//bool Sema::semantic(AST* Tree) {
//  if (!Tree)
//    return true;
//  Tree->accept(Visitor);
//  return Visitor.hasError();
//}