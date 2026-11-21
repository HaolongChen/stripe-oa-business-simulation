#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

CXX="${CXX:-g++}"
"$CXX" -std=c++20 -O2 -Wall -Wextra -pedantic solution.cpp -o solution
./solution < tests/sample.in > /tmp/merchant-ledger-actual.out

if diff -u tests/sample.out /tmp/merchant-ledger-actual.out; then
  echo "Sample test passed."
else
  echo "Sample test failed." >&2
  exit 1
fi
