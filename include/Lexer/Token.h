#ifndef TOKEN_H
#define TOKEN_H

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

enum class TokenKind : short {
  Identifier,
  Number,
  Colon,     // ":'
  Semicolon, // ";"
  Comma,     // ","

  Plus, // "+"
  Minus, // "-"
  Star, // "*"
  Slash, // "/"
  Percent, // "%"
  Assign, // "="

  Equal, // "=="
  NotEqual, // "!="
  Less, // "<"
  LessEq, // "<="
  Greater, // ">"
  GreaterEq, // ">="

  Or, // "||"
  And, // "&&"

  LParen, // "("
  RParen, // ")"
  LFigure, // "{"
  RFigure, // "}"
  LSquare, // "["
  RSquare, // "]"
  Arrow, // "->"

  // Keywords
  KW_integer,
  KW_array,
  KW_if,
  KW_else,
  KW_for,
  KW_while,
  KW_return,
  KW_fun,
  KW_print,
  KW_break,
  KW_continue,

  EOI, // End of input
  Unknown,
};

class Token {
  friend class Lexer;

 private:
  TokenKind Kind;
  llvm::StringRef Text;
  size_t Line;
  size_t Column;

 public:
  [[nodiscard]] TokenKind getKind() const {
    return Kind;
  }

  [[nodiscard]] llvm::StringRef getText() const {
    return Text;
  }

  [[nodiscard]] size_t getLine() const {
    return Line;
  }

  [[nodiscard]] size_t getColumn() const {
    return Column;
  }

  [[nodiscard]] bool is(TokenKind K) const {
    return Kind == K;
  }

  [[nodiscard]] bool isOneOf(TokenKind K1, TokenKind K2) const {
    return is(K1) || is(K2);
  }

  template<typename... Ts>
  bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }
};

#endif //TOKEN_H
