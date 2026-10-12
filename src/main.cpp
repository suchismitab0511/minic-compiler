#include <iostream>
#include <string>
#include "lexer.h"

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

int main(int argc, char** argv) {
    // Debug mode: ./minic --tokens "code"
    if (argc == 3 && std::string(argv[1]) == "--tokens") {
        for (const Token& t : tokenize(argv[2])) {
            std::cout << "line " << t.line << "  "
                      << kindName(t.kind) << "  '" << t.text << "'\n";
        }
        return 0;
    }

    if (argc != 2) {
        std::cerr << "usage: minic <code>  or  minic --tokens <code>\n";
        return 1;
    }

    std::vector<Token> tokens = tokenize(argv[1]);

    // Temporary: still only a single number is supported.
    // The parser in Step 3 replaces this.
    if (tokens[0].kind != TokenKind::Number) {
        error(tokens[0].line, "expected a number");
    }

    std::cout << ".intel_syntax noprefix\n";
    std::cout << ".globl main\n";
    std::cout << "main:\n";
    std::cout << "  mov rax, " << tokens[0].value << "\n";
    std::cout << "  ret\n";
    std::cout << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}