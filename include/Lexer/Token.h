#ifndef TOKEN_H
#define TOKEN_H

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

enum TokenKind : short {
  Identifier,
  Number,
  String,
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

  // Keywords
  KW_integer,
  KW_if,
  KW_else,
  KW_for,
  KW_while,
  KW_return,
  KW_bool,
  KW_true,
  KW_false,

  EOI, // End of input
  Unknown,
};

class Token {
  friend class Lexer;

 private:
  TokenKind Kind;
  llvm::StringRef Text;
 public:
  [[nodiscard]] TokenKind getKind() const {
    return Kind;
  }

  [[nodiscard]] llvm::StringRef getText() const {
    return Text;
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
