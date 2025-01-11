#ifndef SEMA_H
#define SEMA_H

#include "Parser/AST.h"
#include "Lexer/Lexer.h"

class Sema {
 public:
  bool semantic(AST *Tree);
};

#endif //SEMA_H
