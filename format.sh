#!/bin/sh
set -e
format=${CLANG_FORMAT:-clang-format-3.8}
find include src test -name "*.[ch]pp" -exec $format -i --style=file {} \;
