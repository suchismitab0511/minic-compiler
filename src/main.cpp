#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "optimizer.h"

static std::string kindName(TokenKind k) {
    switch (k) {
        case TokenKind::Number:  return "Number";
        case TokenKind::Ident:   return "Ident";
        case TokenKind::Keyword: return "Keyword";
        case TokenKind::Symbol:  return "Symbol";
        case TokenKind::End:     return "End";
    }
    return "?";
}

static void printAst(const Node* n, int depth = 0, const std::string& tag = "") {
    if (!n) return;
    std::cout << std::string(depth * 2, ' ') << tag << nodeLabel(n) << "\n";
    printAst(n->cond.get(), depth + 1, "cond: ");
    printAst(n->then.get(), depth + 1, n->kind == NodeKind::While ? "body: " : "then: ");
    printAst(n->els.get(), depth + 1, "else: ");
    printAst(n->lhs.get(), depth + 1);
    printAst(n->rhs.get(), depth + 1);
    for (const auto& s : n->body) {
        printAst(s.get(), depth + 1);
    }
}

int main(int argc, char** argv) {
    std::string mode, code;
    if (argc == 2) {
        code = argv[1];
    } else if (argc == 3) {
        mode = argv[1];
        code = argv[2];
    } else {
        std::cerr << "usage: minic [--tokens | --ast | --ast-opt | --no-opt] <code>\n";
        return 1;
    }

    if (!mode.empty() && mode != "--tokens" && mode != "--ast" &&
        mode != "--ast-opt" && mode != "--no-opt") {
        std::cerr << "unknown option: " << mode << "\n";
        return 1;
    }

    if (mode == "--tokens") {
        for (const Token& t : tokenize(code)) {
            std::cout << "line " << t.line << "  "
                      << kindName(t.kind) << "  '" << t.text << "'\n";
        }
        return 0;
    }

    Parser parser(tokenize(code));
    Program prog = parser.parseProgram();

    if (mode == "--ast") {              // tree before optimization
        printAst(prog.body.get());
        return 0;
    }

    if (mode != "--no-opt") {
        prog.body = optimize(std::move(prog.body));
    }

    if (mode == "--ast-opt") {          // tree after optimization
        printAst(prog.body.get());
        return 0;
    }

    generate(prog, std::cout);
    return 0;
}