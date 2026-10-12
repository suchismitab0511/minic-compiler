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

echo "ALL TESTS PASSED"