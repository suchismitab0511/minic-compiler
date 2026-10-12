#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: minic <code>\n";
        return 1;
    }

    int value = std::stoi(argv[1]);

    std::cout << ".intel_syntax noprefix\n";
    std::cout << ".globl main\n";
    std::cout << "main:\n";
    std::cout << "  mov rax, " << value << "\n";
    std::cout << "  ret\n";
    std::cout << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}