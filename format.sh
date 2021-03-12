#!/bin/bash -e
find . -regex '.*\.\(h\|hpp\|c\|cpp\)' -exec clang-format -i -style=file {} \;
