#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    for flag in "" "--no-opt"; do
        ./build/minic $flag "$input" > tmp.s || exit 1
        gcc tmp.s -o tmp
        ./tmp
        actual="$?"

        if [ "$actual" != "$expected" ]; then
            echo "FAIL ($flag): $input => expected $expected, got $actual"
            exit 1
        fi
    done
    echo "PASS: $input => $expected"
}

# expressions (from Step 3, now as statements)
assert 0 "return 0;"
assert 42 "return 42;"
assert 21 "return 5+20-4;"
assert 14 "return 2+3*4;"
assert 20 "return (2+3)*4;"
assert 5 "return 10-3-2;"
assert 4 "return (3+5)/2;"
assert 7 "return 10/3*2+1;"
assert 10 "return -10+20;"
assert 5 "return - -5;"
assert 1 "return 1==1;"
assert 0 "return 1!=1;"
assert 1 "return 2<3;"
assert 0 "return 3<2;"
assert 1 "return 3<=3;"
assert 1 "return 4>3;"
assert 0 "return 3>=4;"
assert 1 "return !0;"
assert 0 "return !5;"
assert 1 "return (1+2)*3==9;"

# variables and statements
assert 3 "int a = 3; return a;"
assert 8 "int a = 3; int b = 5; return a + b;"
assert 6 "int a; int b; a = b = 3; return a + b;"
assert 14 "int x = 3; int y = 5 * 2 - 6; return x * y + 2;"
assert 10 "int x = 5; x = x * 2; return x;"
assert 2 "int foo = 1; int bar = foo + 1; return bar;"
assert 5 "{ int a = 5; return a; }"
assert 0 "int a = 7;"
assert 1 "return 1; return 2;"
# if / else
assert 3 "if (0) return 2; return 3;"
assert 3 "if (1-1) return 2; return 3;"
assert 2 "if (1) return 2; return 3;"
assert 2 "if (2-1) return 2; return 3;"
assert 4 "if (0) return 3; else return 4;"
assert 3 "int x = 5; if (x > 3) return 3; else return 4;"
assert 2 "int x = 0; if (1) if (0) x = 1; else x = 2; return x;"

# while
assert 10 "int i = 0; while (i < 10) i = i + 1; return i;"
assert 0 "while (0) return 1; return 0;"
assert 55 "int i = 0; int sum = 0; while (i <= 10) { sum = sum + i; i = i + 1; } return sum;"
assert 120 "int n = 5; int f = 1; while (n > 1) { f = f * n; n = n - 1; } return f;"

# nested loops and real algorithms
assert 6 "int i = 0; int c = 0; while (i < 3) { int j = 0; while (j < 2) { c = c + 1; j = j + 1; } i = i + 1; } return c;"
assert 89 "int a = 0; int b = 1; int i = 0; while (i < 10) { int t = a + b; a = b; b = t; i = i + 1; } return b;"
assert 6 "int a = 48; int b = 18; while (b != 0) { int t = b; b = a - a / b * b; a = t; } return a;"

# optimizer
assert 10 "int x = 2 * 3 + 4; return x;"
assert 7 "if (1) return 7; return 9;"
assert 9 "if (2 > 3) return 1; return 9;"
assert 9 "while (0) { return 1; } return 9;"
assert 0 "int x = 0; if (0) x = 1 / 0; return x;"
assert 1 "return 2000000000 + 2000000000 > 0;"

echo "ALL TESTS PASSED"
