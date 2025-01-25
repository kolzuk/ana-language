#ifndef SCOPE_H
#define SCOPE_H

#include "Parser/AST.h"
#include <map>
#include <utility>


class Decl {
 public:
  std::string Name;
  TypeAST* Tp;
  bool IsInitialized;
  ArgumentsListAST* Arguments;
  Decl(std::string name, TypeAST* type, bool isInitialized, ArgumentsListAST* arguments);
};

class Scope {
  std::string Name;
  Scope* Parent;
  std::map<std::string, Decl*> ScopeMembers;
 public:
  explicit Scope(std::string Name, Scope *Parent = nullptr)
      : Name(std::move(Name)),
        Parent(Parent)
  {}
  bool insert(Decl *Declaration);
  Decl* lookup(std::string Name);
  Scope* getParent();
  std::string getName();
  Decl* getUpFunctionDecl();
};

#endif //SCOPE_H