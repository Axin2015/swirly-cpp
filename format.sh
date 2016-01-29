#!/bin/sh
find . -name "*.[ch]pp" -exec clang-format -i --style=file {} \;
