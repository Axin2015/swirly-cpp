#!/bin/sh
set -e
format=${CLANG_FORMAT:-clang-format}
find example include src test \( -name "*.[ch]pp" -a ! -name "RestBody.cpp" \) \
     -exec $format -i --style=file {} \;
