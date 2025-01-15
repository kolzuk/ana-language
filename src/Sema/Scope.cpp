#include "Sema/Scope.h"

Decl *Scope::lookup(StringRef Name) {
  Scope *S = this;
  while (S) {
    llvm::StringMap<Decl *>::const_iterator I = S->Symbols.find(Name);
    if (I != S->Symbols.end())
      return I->second;
    S = S->getParent();
  }
  return nullptr;
}