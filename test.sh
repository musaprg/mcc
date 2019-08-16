#!/bin/sh

try() {
  expected="$1"
  input="$2"
  ./mcc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"
  if [ "$actual" != "$expected" ]; then
    echo "$input expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 42 42
# test add and sub
try 21 "5+20-4"
# test accept brank space
try 41 " 12 + 34 - 5 "
echo OK
