#ifndef LEXER_H
#define LEXER_H

#include <cstdint>
#include <deque>
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
  Token peek(int n);

 private:
  void formToken(Token& Result, const char* TokEnd, TokenKind Kind);
  std::deque<Token> TokensBuffer;
};

#endif //LEXER_H
