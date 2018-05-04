#!/bin/sh
set -e
export PATH=/usr/share/clang:$PATH
build=${SWIRLY_BUILD:-$HOME/build/swirly}
exec run-clang-tidy.py -header-filter='.*' -fix -p=$build
