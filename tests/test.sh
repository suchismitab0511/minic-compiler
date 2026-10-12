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

assert 0 "0"
assert 42 "42"
assert 21 "5+20-4"
assert 14 "2+3*4"
assert 20 "(2+3)*4"
assert 5 "10-3-2"
assert 4 "(3+5)/2"
assert 7 "10/3*2+1"
assert 10 "-10+20"
assert 5 "- -5"
assert 1 "1==1"
assert 0 "1!=1"
assert 1 "2<3"
assert 0 "3<2"
assert 1 "3<=3"
assert 1 "4>3"
assert 0 "3>=4"
assert 1 "!0"
assert 0 "!5"
assert 1 "(1+2)*3==9"

echo "ALL TESTS PASSED"
