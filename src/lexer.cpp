#include "lexer.h"
#include <cctype>
#include <cstdlib>
#include <iostream>

void error(int line, const std::string& msg) {
    std::cerr << "error (line " << line << "): " << msg << "\n";
    std::exit(1);
}

static bool isKeyword(const std::string& s) {
    return s == "int" || s == "return" || s == "if" ||
           s == "else" || s == "while";
}

std::vector<Token> tokenize(const std::string& src) {
    std::vector<Token> tokens;
    size_t i = 0;
    int line = 1;

    while (i < src.size()) {
        unsigned char c = src[i];

        // 1. Newlines and spaces
        if (c == '\n') { line++; i++; continue; }
        if (std::isspace(c)) { i++; continue; }

        // 2. Numbers
        if (std::isdigit(c)) {
            size_t start = i;
            while (i < src.size() && std::isdigit((unsigned char)src[i])) i++;
            std::string text = src.substr(start, i - start);
            tokens.push_back({TokenKind::Number, text, std::stoi(text), line});
            continue;
        }

        // 3. Keywords and identifiers
        if (std::isalpha(c) || c == '_') {
            size_t start = i;
            while (i < src.size() &&
                   (std::isalnum((unsigned char)src[i]) || src[i] == '_')) i++;
            std::string text = src.substr(start, i - start);
            TokenKind kind = isKeyword(text) ? TokenKind::Keyword : TokenKind::Ident;
            tokens.push_back({kind, text, 0, line});
            continue;
        }

        // 4. Two-character symbols (checked first: maximal munch)
        if (i + 1 < src.size()) {
            std::string two = src.substr(i, 2);
            if (two == "==" || two == "!=" || two == "<=" || two == ">=") {
                tokens.push_back({TokenKind::Symbol, two, 0, line});
                i += 2;
                continue;
            }
        }

        // 5. One-character symbols
        if (std::string("+-*/()<>{};=!").find(c) != std::string::npos) {
            tokens.push_back({TokenKind::Symbol, std::string(1, c), 0, line});
            i++;
            continue;
        }

        // 6. Anything else is an error
        error(line, std::string("unexpected character '") + (char)c + "'");
    }

    tokens.push_back({TokenKind::End, "", 0, line});
    return tokens;
}