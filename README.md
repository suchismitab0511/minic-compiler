# MiniC

A compiler for a subset of C, written in C++17. It turns source code into
x86-64 assembly, which GCC assembles and links into a native Linux executable.

## Features

- Integer arithmetic: `+ - * /`, unary `-` and `!`
- Comparisons: `== != < <= > >=`
- Local variables with declaration and assignment (`int x = 5;`, `a = b = 3;`)
- Control flow: `if` / `else`, `while`, `{ }` blocks, `return`
- Error messages with line numbers

## How it works

```
source code
   │  Lexer          → splits text into tokens
   ▼
tokens
   │  Parser         → recursive descent, builds an Abstract Syntax Tree
   ▼
AST
   │  Code generator → x86-64 assembly (Intel syntax)
   ▼
assembly (.s)
   │  GCC assembler + linker
   ▼
native executable
```

## Example

Greatest common divisor of 48 and 18 (Euclid's algorithm):

```bash
./build/minic "int a = 48; int b = 18; while (b != 0) { int t = b; b = a - a / b * b; a = t; } return a;" > gcd.s
gcc gcd.s -o gcd
./gcd; echo $?     # prints 6
```

### Viewing the AST

```bash
./build/minic --ast "int x = 2; while (x < 5) x = x + 1; return x;"
```

```
Block
  ExprStmt
    Assign
      Var x (rbp-8)
      Num 2
  While
    cond: Lt
      Var x (rbp-8)
      Num 5
    body: ExprStmt
      Assign
        Var x (rbp-8)
        Add
          Var x (rbp-8)
          Num 1
  Return
    Var x (rbp-8)
```

## Build and run

Requires Linux (or WSL), CMake 3.16+ and GCC.

```bash
cmake -S . -B build
cmake --build build
./build/minic "int x = 3; return x * 4;" > out.s
gcc out.s -o out && ./out; echo $?     # prints 12
```

## Options

| Option     | What it does                 |
|------------|------------------------------|
| (none)     | Compile code to assembly     |
| `--tokens` | Print the token list         |
| `--ast`    | Print the syntax tree        |

## Testing

```bash
./tests/test.sh
```

43 end-to-end tests. Each test compiles a program, assembles and runs it,
and checks its exit code. The tests cover operator precedence and
associativity, variables, nested control flow, and small algorithms
(factorial, Fibonacci, GCD).

## Design decisions

- **Recursive descent parser:** one function per precedence level. Loops give
  left associativity (`10-3-2` is `(10-3)-2`); recursion gives right
  associativity for assignment (`a = b = 3`).
- **Stack-based code generation:** every expression leaves its result in `rax`,
  and temporary values are saved with `push`/`pop`. Simple and correct, at the
  cost of extra memory traffic compared with register allocation.
- **Stack frame:** local variables live at fixed offsets from `rbp`. The frame
  size is rounded up to 16 bytes, as required by the System V ABI.
- **Control flow:** `if/else` and `while` are lowered to conditional and
  unconditional jumps between uniquely numbered labels. All `return`
  statements jump to a single shared epilogue.

## Limitations

- Only `int`; no pointers, arrays or other types
- No user-defined functions; the program body becomes `main`
- One flat scope for all variables
- No register allocation (stack-based code generation)

## Future work

- Optimization passes: constant folding and dead code elimination
- Functions and calls using the System V calling convention
- Block scopes
- An SSA-based intermediate representation

## References

- Rui Ueyama, *chibicc*: a small C compiler
- Nora Sandler, *Writing a C Compiler*