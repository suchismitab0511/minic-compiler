#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

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
    if (argc == 3) {
        std::string mode = argv[1];
        if (mode == "--tokens") {
            for (const Token& t : tokenize(argv[2])) {
                std::cout << "line " << t.line << "  "
                          << kindName(t.kind) << "  '" << t.text << "'\n";
            }
            return 0;
        }
        if (mode == "--ast") {
            Parser parser(tokenize(argv[2]));
            Program prog = parser.parseProgram();
            printAst(prog.body.get());
            return 0;
        }
    }

    if (argc != 2) {
        std::cerr << "usage: minic <code> | minic --tokens <code> | minic --ast <code>\n";
        return 1;
    }

    Parser parser(tokenize(argv[1]));
    Program prog = parser.parseProgram();
    generate(prog, std::cout);
    return 0;
}