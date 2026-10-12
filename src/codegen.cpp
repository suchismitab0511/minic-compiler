#include "codegen.h"
#include "lexer.h"   // for error()

static void genExpr(const Node* n, std::ostream& out) {
    switch (n->kind) {
    case NodeKind::Num:
        out << "  mov rax, " << n->value << "\n";
        return;
    case NodeKind::Var:
        out << "  mov rax, [rbp-" << n->offset << "]\n";
        return;
    case NodeKind::Assign:
        genExpr(n->rhs.get(), out);                              // value -> rax
        out << "  mov [rbp-" << n->lhs->offset << "], rax\n";   // store it
        return;                                                  // rax still holds value
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
        break;
    }

    // binary nodes
    genExpr(n->rhs.get(), out);
    out << "  push rax\n";
    genExpr(n->lhs.get(), out);
    out << "  pop rdi\n";

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
        error(0, "codegen: unknown expression");
    }
}

static void genStmt(const Node* n, std::ostream& out) {
    switch (n->kind) {
    case NodeKind::Return:
        genExpr(n->lhs.get(), out);
        out << "  jmp .L.return\n";
        return;
    case NodeKind::ExprStmt:
        genExpr(n->lhs.get(), out);   // result is ignored
        return;
    case NodeKind::Block:
        for (const auto& s : n->body) {
            genStmt(s.get(), out);
        }
        return;
    default:
        error(0, "codegen: unknown statement");
    }
}

void generate(const Program& prog, std::ostream& out) {
    int frameSize = (prog.stackSize + 15) / 16 * 16;   // round up to 16

    out << ".intel_syntax noprefix\n";
    out << ".globl main\n";
    out << "main:\n";

    // prologue
    out << "  push rbp\n";
    out << "  mov rbp, rsp\n";
    out << "  sub rsp, " << frameSize << "\n";

    genStmt(prog.body.get(), out);

    // default return value if the program has no 'return'
    out << "  mov rax, 0\n";

    // epilogue
    out << ".L.return:\n";
    out << "  mov rsp, rbp\n";
    out << "  pop rbp\n";
    out << "  ret\n";
    out << ".section .note.GNU-stack,\"\",@progbits\n";
}