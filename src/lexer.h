#pragma once
#include <string>
#include <vector>

enum class TokenKind { Number, Ident, Keyword, Symbol, End };

struct Token {
    TokenKind kind;
    std::string text;
    int value = 0;   // only used for Number tokens
    int line = 1;    // for error messages
};

std::vector<Token> tokenize(const std::string& src);

[[noreturn]] void error(int line, const std::string& msg);