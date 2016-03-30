#!/bin/sh
set -e
build=${SWIRLY_BUILD:-$HOME/build/swirlyc}
tidy=${CLANG_FORMAT:-clang-tidy-3.8}
for f in $(find src test -name "*.cpp"); do
  $tidy -p=$build $f
  #$tidy --checks="-*,readability-braces-around-statements" --fix -p=$build $f
done
