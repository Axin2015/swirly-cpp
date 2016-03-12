#!/bin/sh
find include src test -name "*.[ch]pp" -exec clang-format -i --style=file {} \;
