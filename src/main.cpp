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

static void printAst(const Node* n, int depth = 0) {
    if (!n) return;
    std::cout << std::string(depth * 2, ' ') << nodeLabel(n) << "\n";
    printAst(n->lhs.get(), depth + 1);
    printAst(n->rhs.get(), depth + 1);
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
            auto root = parser.parseProgram();
            printAst(root.get());
            return 0;
        }
    }

    if (argc != 2) {
        std::cerr << "usage: minic <code> | minic --tokens <code> | minic --ast <code>\n";
        return 1;
    }

    Parser parser(tokenize(argv[1]));
    auto root = parser.parseProgram();
    generate(root.get(), std::cout);
    return 0;
}