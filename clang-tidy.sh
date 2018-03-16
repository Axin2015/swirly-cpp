#!/bin/sh
set -e
build=${SWIRLY_BUILD:-$HOME/build/swirly}
tidy=${CLANG_FORMAT:-clang-tidy}
for f in $(find src test -name "*.cpp"); do
  $tidy -p=$build $f
  #$tidy --checks="-*,readability-braces-around-statements" --fix -p=$build $f
done
