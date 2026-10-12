#include "codegen.h"
#include "lexer.h"   // for error()

static void genExpr(const Node* n, std::ostream& out) {
    // --- leaf and unary nodes ---
    switch (n->kind) {
    case NodeKind::Num:
        out << "  mov rax, " << n->value << "\n";
        return;
    case NodeKind::Neg:
        genExpr(n->lhs.get(), out);
        out << "  neg rax\n";
        return;
    case NodeKind::Not:
        genExpr(n->lhs.get(), out);
        out << "  cmp rax, 0\n";
        out << "  sete al\n";
        out << "  movzx rax, al\n";
        return;
    default:
        break;   // binary node: handled below
    }

    // --- binary nodes: right side, save, left side, restore ---
    genExpr(n->rhs.get(), out);
    out << "  push rax\n";
    genExpr(n->lhs.get(), out);
    out << "  pop rdi\n";
    // now: rax = left, rdi = right

    switch (n->kind) {
    case NodeKind::Add: out << "  add rax, rdi\n"; break;
    case NodeKind::Sub: out << "  sub rax, rdi\n"; break;
    case NodeKind::Mul: out << "  imul rax, rdi\n"; break;
    case NodeKind::Div:
        out << "  cqo\n";
        out << "  idiv rdi\n";
        break;
    case NodeKind::Eq:
    case NodeKind::Ne:
    case NodeKind::Lt:
    case NodeKind::Le:
        out << "  cmp rax, rdi\n";
        if (n->kind == NodeKind::Eq) out << "  sete al\n";
        if (n->kind == NodeKind::Ne) out << "  setne al\n";
        if (n->kind == NodeKind::Lt) out << "  setl al\n";
        if (n->kind == NodeKind::Le) out << "  setle al\n";
        out << "  movzx rax, al\n";
        break;
    default:
        error(0, "codegen: unknown node");
    }
}

void generate(const Node* root, std::ostream& out) {
    out << ".intel_syntax noprefix\n";
    out << ".globl main\n";
    out << "main:\n";
    genExpr(root, out);
    out << "  ret\n";
    out << ".section .note.GNU-stack,\"\",@progbits\n";
}