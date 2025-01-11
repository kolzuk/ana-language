#ifndef LEXER_H
#define LEXER_H

#include <cstdint>
#include <map>
#include <string>

#include "Token.h"

class Lexer {
  const char* BufferStart;
  const char* BufferPtr;

 public:
  explicit Lexer(const llvm::StringRef& Buffer) {
    BufferStart = Buffer.begin();
    BufferPtr = BufferStart;
  }

  void next(Token& Token);

 private:
  void formToken(Token& Result, const char* TokEnd, TokenKind Kind);
};

#endif //LEXER_H
