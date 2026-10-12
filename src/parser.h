#pragma once
#include <memory>
#include <string>
#include <vector>
#include "ast.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : toks(std::move(tokens)) {}
    std::unique_ptr<Node> parseProgram();

private:
    std::vector<Token> toks;
    size_t pos = 0;   // index of the current token

    const Token& peek() const { return toks[pos]; }
    bool match(const std::string& text);
    void expect(const std::string& text);

    std::unique_ptr<Node> expr();
    std::unique_ptr<Node> equality();
    std::unique_ptr<Node> relational();
    std::unique_ptr<Node> add();
    std::unique_ptr<Node> mul();
    std::unique_ptr<Node> unary();
    std::unique_ptr<Node> primary();
};