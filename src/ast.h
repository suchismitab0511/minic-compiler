#pragma once
#include <memory>
#include <string>

enum class NodeKind {
    Num,                    // number
    Neg, Not,               // unary: -x, !x
    Add, Sub, Mul, Div,     // arithmetic
    Eq, Ne, Lt, Le          // comparisons (> and >= become Lt/Le)
};

struct Node {
    NodeKind kind;
    int value = 0;                // only for Num
    std::unique_ptr<Node> lhs;    // left child (or the only child for Neg/Not)
    std::unique_ptr<Node> rhs;    // right child
};

inline std::unique_ptr<Node> makeNum(int v) {
    auto n = std::make_unique<Node>();
    n->kind = NodeKind::Num;
    n->value = v;
    return n;
}

inline std::unique_ptr<Node> makeUnary(NodeKind k, std::unique_ptr<Node> operand) {
    auto n = std::make_unique<Node>();
    n->kind = k;
    n->lhs = std::move(operand);
    return n;
}

inline std::unique_ptr<Node> makeBinary(NodeKind k,
                                        std::unique_ptr<Node> l,
                                        std::unique_ptr<Node> r) {
    auto n = std::make_unique<Node>();
    n->kind = k;
    n->lhs = std::move(l);
    n->rhs = std::move(r);
    return n;
}

inline std::string nodeLabel(const Node* n) {
    switch (n->kind) {
        case NodeKind::Num: return "Num " + std::to_string(n->value);
        case NodeKind::Neg: return "Neg";
        case NodeKind::Not: return "Not";
        case NodeKind::Add: return "Add";
        case NodeKind::Sub: return "Sub";
        case NodeKind::Mul: return "Mul";
        case NodeKind::Div: return "Div";
        case NodeKind::Eq:  return "Eq";
        case NodeKind::Ne:  return "Ne";
        case NodeKind::Lt:  return "Lt";
        case NodeKind::Le:  return "Le";
    }
    return "?";
}