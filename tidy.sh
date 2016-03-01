#!/bin/sh
# Run cmake with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
build=${SWIRLY_BUILD:-$HOME/build/swirlyc}
for f in $(find . -name "*.cpp"); do
  clang-tidy -p=$build $f
  #clang-tidy --checks="-*,readability-braces-around-statements" --fix -p=$build $f
done
