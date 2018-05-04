#!/bin/sh
set -e
export PATH=/usr/share/clang:$PATH
build=${SWIRLY_BUILD:-$HOME/build/swirly}
run-clang-tidy.py -header-filter='.*' -fix -p=$build
git checkout -f src/swirly/contrib src/swirly/web/RestBody.cpp
