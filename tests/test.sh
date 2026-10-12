#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    ./build/minic "$input" > tmp.s || exit 1
    gcc tmp.s -o tmp
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "PASS: $input => $actual"
    else
        echo "FAIL: $input => expected $expected, got $actual"
        exit 1
    fi
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

echo "ALL TESTS PASSED"
