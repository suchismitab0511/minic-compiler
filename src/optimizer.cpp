#include "optimizer.h"
#include <climits>

static bool isNum(const std::unique_ptr<Node>& n) {
    return n && n->kind == NodeKind::Num;
}

static bool fitsInInt(long long v) {
    return v >= INT_MIN && v <= INT_MAX;
}

// Constant folding for one node whose children are already optimized.
static std::unique_ptr<Node> fold(std::unique_ptr<Node> n) {
    // Unary: -5 -> Num -5,  !0 -> Num 1
    if ((n->kind == NodeKind::Neg || n->kind == NodeKind::Not) && isNum(n->lhs)) {
        long long v = n->lhs->value;
        long long r = (n->kind == NodeKind::Neg) ? -v : (v == 0);
        if (fitsInInt(r)) return makeNum((int)r);
        return n;
    }

    // Binary: both sides must be numbers
    if (!isNum(n->lhs) || !isNum(n->rhs)) return n;

    long long a = n->lhs->value;
    long long b = n->rhs->value;
    long long r;

    switch (n->kind) {
    case NodeKind::Add: r = a + b; break;
    case NodeKind::Sub: r = a - b; break;
    case NodeKind::Mul: r = a * b; break;
    case NodeKind::Div:
        if (b == 0) return n;      // don't fold: leave the error to runtime
        r = a / b;                 // C++ truncates toward zero, same as idiv
        break;
    case NodeKind::Eq: r = (a == b); break;
    case NodeKind::Ne: r = (a != b); break;
    case NodeKind::Lt: r = (a < b);  break;
    case NodeKind::Le: r = (a <= b); break;
    default: return n;             // e.g. Assign: not foldable
    }

    if (!fitsInInt(r)) return n;   // don't fold: result would change
    return makeNum((int)r);
}

std::unique_ptr<Node> optimize(std::unique_ptr<Node> n) {
    if (!n) return n;

    // 1. Optimize all children first (post-order).
    n->lhs  = optimize(std::move(n->lhs));
    n->rhs  = optimize(std::move(n->rhs));
    n->cond = optimize(std::move(n->cond));
    n->then = optimize(std::move(n->then));
    n->els  = optimize(std::move(n->els));
    for (auto& s : n->body) {
        s = optimize(std::move(s));
    }

    // 2. Then optimize this node.
    switch (n->kind) {
    case NodeKind::If:
        if (isNum(n->cond)) {
            if (n->cond->value != 0) return std::move(n->then);   // always true
            if (n->els) return std::move(n->els);                 // always false
            return makeNode(NodeKind::Block);                     // false, no else
        }
        return n;

    case NodeKind::While:
        if (isNum(n->cond) && n->cond->value == 0) {
            return makeNode(NodeKind::Block);                     // never runs
        }
        return n;

    case NodeKind::Block:
        // Remove every statement after the first 'return'.
        for (size_t i = 0; i < n->body.size(); i++) {
            if (n->body[i]->kind == NodeKind::Return) {
                n->body.erase(n->body.begin() + i + 1, n->body.end());
                break;
            }
        }
        return n;

    default:
        return fold(std::move(n));
    }
}