#include "parser.h"

// ---------- helpers ----------

bool Parser::match(const std::string& text) {
    const Token& t = peek();
    if ((t.kind == TokenKind::Symbol || t.kind == TokenKind::Keyword) &&
        t.text == text) {
        pos++;
        return true;
    }
    return false;
}

void Parser::expect(const std::string& text) {
    if (!match(text)) {
        std::string found = peek().kind == TokenKind::End ? "end of input"
                                                          : "'" + peek().text + "'";
        error(peek().line, "expected '" + text + "' but found " + found);
    }
}

// ---------- entry point ----------

std::unique_ptr<Node> Parser::parseProgram() {
    auto node = expr();
    if (peek().kind != TokenKind::End) {
        error(peek().line, "unexpected '" + peek().text + "'");
    }
    return node;
}

// ---------- grammar rules (lowest to highest precedence) ----------

// expr = equality
std::unique_ptr<Node> Parser::expr() {
    return equality();
}

// equality = relational ( ("==" | "!=") relational )*
std::unique_ptr<Node> Parser::equality() {
    auto node = relational();
    while (true) {
        if (match("==")) {
            auto rhs = relational();
            node = makeBinary(NodeKind::Eq, std::move(node), std::move(rhs));
        } else if (match("!=")) {
            auto rhs = relational();
            node = makeBinary(NodeKind::Ne, std::move(node), std::move(rhs));
        } else {
            return node;
        }
    }
}

// relational = add ( ("<" | "<=" | ">" | ">=") add )*
std::unique_ptr<Node> Parser::relational() {
    auto node = add();
    while (true) {
        if (match("<")) {
            auto rhs = add();
            node = makeBinary(NodeKind::Lt, std::move(node), std::move(rhs));
        } else if (match("<=")) {
            auto rhs = add();
            node = makeBinary(NodeKind::Le, std::move(node), std::move(rhs));
        } else if (match(">")) {           // a > b  ==>  b < a
            auto rhs = add();
            node = makeBinary(NodeKind::Lt, std::move(rhs), std::move(node));
        } else if (match(">=")) {          // a >= b ==>  b <= a
            auto rhs = add();
            node = makeBinary(NodeKind::Le, std::move(rhs), std::move(node));
        } else {
            return node;
        }
    }
}

// add = mul ( ("+" | "-") mul )*
std::unique_ptr<Node> Parser::add() {
    auto node = mul();
    while (true) {
        if (match("+")) {
            auto rhs = mul();
            node = makeBinary(NodeKind::Add, std::move(node), std::move(rhs));
        } else if (match("-")) {
            auto rhs = mul();
            node = makeBinary(NodeKind::Sub, std::move(node), std::move(rhs));
        } else {
            return node;
        }
    }
}

// mul = unary ( ("*" | "/") unary )*
std::unique_ptr<Node> Parser::mul() {
    auto node = unary();
    while (true) {
        if (match("*")) {
            auto rhs = unary();
            node = makeBinary(NodeKind::Mul, std::move(node), std::move(rhs));
        } else if (match("/")) {
            auto rhs = unary();
            node = makeBinary(NodeKind::Div, std::move(node), std::move(rhs));
        } else {
            return node;
        }
    }
}

// unary = ("-" | "!" | "+") unary | primary
std::unique_ptr<Node> Parser::unary() {
    if (match("-")) return makeUnary(NodeKind::Neg, unary());
    if (match("!")) return makeUnary(NodeKind::Not, unary());
    if (match("+")) return unary();      // +x is just x
    return primary();
}

// primary = number | "(" expr ")"
std::unique_ptr<Node> Parser::primary() {
    if (match("(")) {
        auto node = expr();               // brackets restart from the lowest level
        expect(")");
        return node;
    }
    if (peek().kind == TokenKind::Number) {
        int v = peek().value;
        pos++;
        return makeNum(v);
    }
    error(peek().line, "expected a number or '('");
}