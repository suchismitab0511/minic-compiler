#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ast.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : toks(std::move(tokens)) {}
    Program parseProgram();

private:
    std::vector<Token> toks;
    size_t pos = 0;

    // variable table: name -> offset from rbp
    std::unordered_map<std::string, int> locals;
    int stackSize = 0;

    const Token& peek() const { return toks[pos]; }
    bool match(const std::string& text);
    void expect(const std::string& text);
    std::string expectIdent();

    std::unique_ptr<Node> stmt();
    std::unique_ptr<Node> declaration();
    std::unique_ptr<Node> expr();
    std::unique_ptr<Node> assign();
    std::unique_ptr<Node> equality();
    std::unique_ptr<Node> relational();
    std::unique_ptr<Node> add();
    std::unique_ptr<Node> mul();
    std::unique_ptr<Node> unary();
    std::unique_ptr<Node> primary();
};