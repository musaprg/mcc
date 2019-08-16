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
try 47 '5+6*7'
try 15 '5*(9-6)'
try 4 '(3+5)/2'
echo OK
