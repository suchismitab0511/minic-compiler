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

std::string Parser::expectIdent() {
    if (peek().kind != TokenKind::Ident) {
        error(peek().line, "expected a variable name");
    }
    return toks[pos++].text;
}

// ---------- entry point ----------

// program = stmt*
Program Parser::parseProgram() {
    auto block = makeNode(NodeKind::Block);
    while (peek().kind != TokenKind::End) {
        block->body.push_back(stmt());
    }
    Program prog;
    prog.body = std::move(block);
    prog.stackSize = stackSize;
    return prog;
}

// ---------- statements ----------

std::unique_ptr<Node> Parser::stmt() {
    // "return" expr ";"
    if (match("return")) {
        auto node = makeUnary(NodeKind::Return, expr());
        expect(";");
        return node;
    }

    // "int" ident ("=" expr)? ";"
    if (match("int")) {
        return declaration();
    }

    // "{" stmt* "}"
    if (match("{")) {
        auto block = makeNode(NodeKind::Block);
        while (!match("}")) {
            if (peek().kind == TokenKind::End) {
                error(peek().line, "expected '}' but found end of input");
            }
            block->body.push_back(stmt());
        }
        return block;
    }

    // expr ";"
    auto node = makeUnary(NodeKind::ExprStmt, expr());
    expect(";");
    return node;
}

// Called after "int" has been read.
std::unique_ptr<Node> Parser::declaration() {
    int line = peek().line;
    std::string name = expectIdent();

    if (locals.count(name)) {
        error(line, "variable '" + name + "' is already declared");
    }
    stackSize += 8;
    locals[name] = stackSize;          // first var: 8, second: 16, ...

    if (match("=")) {
        auto var = makeVar(name, locals[name]);
        auto value = expr();
        expect(";");
        return makeUnary(NodeKind::ExprStmt,
                         makeBinary(NodeKind::Assign, std::move(var), std::move(value)));
    }

    expect(";");
    return makeNode(NodeKind::Block);   // "int x;" alone generates no code
}

// ---------- expressions (lowest to highest precedence) ----------

// expr = assign
std::unique_ptr<Node> Parser::expr() {
    return assign();
}

// assign = equality ("=" assign)?
std::unique_ptr<Node> Parser::assign() {
    auto node = equality();
    if (match("=")) {
        if (node->kind != NodeKind::Var) {
            error(peek().line, "left side of '=' must be a variable");
        }
        auto rhs = assign();           // calls itself: right-associative
        node = makeBinary(NodeKind::Assign, std::move(node), std::move(rhs));
    }
    return node;
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
    if (match("+")) return unary();
    return primary();
}

// primary = number | ident | "(" expr ")"
std::unique_ptr<Node> Parser::primary() {
    if (match("(")) {
        auto node = expr();
        expect(")");
        return node;
    }
    if (peek().kind == TokenKind::Number) {
        int v = peek().value;
        pos++;
        return makeNum(v);
    }
    if (peek().kind == TokenKind::Ident) {
        const Token& t = peek();
        auto it = locals.find(t.text);
        if (it == locals.end()) {
            error(t.line, "undeclared variable '" + t.text + "'");
        }
        pos++;
        return makeVar(t.text, it->second);
    }
    error(peek().line, "expected a number, variable or '('");
}