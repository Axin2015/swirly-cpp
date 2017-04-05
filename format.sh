#!/bin/sh
set -e
format=${CLANG_FORMAT:-clang-format}
find example src \( ! -name "RestBody.cpp" -a -name "*.[ch]pp" -o -name "*.[ch]xx" \) \
     -exec $format -i --style=file {} \;
