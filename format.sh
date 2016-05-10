#!/bin/sh
set -e
format=${CLANG_FORMAT:-clang-format}
find example include src test \( -name "*.[ch]pp" -a ! -name "RestRequest.cpp" \) \
     -exec $format -i --style=file {} \;
