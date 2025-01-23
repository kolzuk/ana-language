#ifndef SCOPE_H
#define SCOPE_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "Parser/AST.h"


class Decl {
public:
  llvm::StringRef Name;
  TypeAST* Tp;
  bool IsInitialized;
  ArgumentsListAST* Arguments;
  Decl(llvm::StringRef name, TypeAST* type, bool isInitialized, ArgumentsListAST* arguments);
};

class Scope {
  llvm::StringRef Name;
  Scope* Parent;
  llvm::StringMap<Decl*> ScopeMembers;
public:
  explicit Scope(llvm::StringRef Name, Scope *Parent = nullptr)
  : Name(Name),
    Parent(Parent)
  {}
  bool insert(Decl *Declaration);
  Decl* lookup(llvm::StringRef Name);
  Scope* getParent();
  llvm::StringRef getName();
  Decl* getUpFunctionDecl();
  ;
};

#endif //SCOPE_H
