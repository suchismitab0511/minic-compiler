#pragma once
#include <memory>
#include <string>
#include <vector>

enum class NodeKind {
    // expressions
    Num, Var,
    Neg, Not,
    Add, Sub, Mul, Div,
    Eq, Ne, Lt, Le,
    Assign,
    // statements
    Return, ExprStmt, Block
};

struct Node {
    NodeKind kind;
    int value = 0;                              // Num
    std::string name;                           // Var
    int offset = 0;                             // Var: lives at [rbp - offset]
    std::unique_ptr<Node> lhs;                  // left child / only child
    std::unique_ptr<Node> rhs;                  // right child
    std::vector<std::unique_ptr<Node>> body;    // Block: list of statements
};

// The whole program: the statements plus how much stack space they need.
struct Program {
    std::unique_ptr<Node> body;
    int stackSize = 0;
};

inline std::unique_ptr<Node> makeNode(NodeKind k) {
    auto n = std::make_unique<Node>();
    n->kind = k;
    return n;
}

inline std::unique_ptr<Node> makeNum(int v) {
    auto n = makeNode(NodeKind::Num);
    n->value = v;
    return n;
}

inline std::unique_ptr<Node> makeVar(const std::string& name, int offset) {
    auto n = makeNode(NodeKind::Var);
    n->name = name;
    n->offset = offset;
    return n;
}

inline std::unique_ptr<Node> makeUnary(NodeKind k, std::unique_ptr<Node> operand) {
    auto n = makeNode(k);
    n->lhs = std::move(operand);
    return n;
}

inline std::unique_ptr<Node> makeBinary(NodeKind k,
                                        std::unique_ptr<Node> l,
                                        std::unique_ptr<Node> r) {
    auto n = makeNode(k);
    n->lhs = std::move(l);
    n->rhs = std::move(r);
    return n;
}

inline std::string nodeLabel(const Node* n) {
    switch (n->kind) {
        case NodeKind::Num:      return "Num " + std::to_string(n->value);
        case NodeKind::Var:      return "Var " + n->name + " (rbp-" + std::to_string(n->offset) + ")";
        case NodeKind::Neg:      return "Neg";
        case NodeKind::Not:      return "Not";
        case NodeKind::Add:      return "Add";
        case NodeKind::Sub:      return "Sub";
        case NodeKind::Mul:      return "Mul";
        case NodeKind::Div:      return "Div";
        case NodeKind::Eq:       return "Eq";
        case NodeKind::Ne:       return "Ne";
        case NodeKind::Lt:       return "Lt";
        case NodeKind::Le:       return "Le";
        case NodeKind::Assign:   return "Assign";
        case NodeKind::Return:   return "Return";
        case NodeKind::ExprStmt: return "ExprStmt";
        case NodeKind::Block:    return "Block";
    }
    return "?";
}