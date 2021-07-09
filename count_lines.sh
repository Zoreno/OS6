#!/bin/bash

echo -n "Files: "
find . -not -path ./build -iregex '.*\.\(c\|h\|cpp\|hpp\|asm\)$' | wc -l

echo -n "Lines: "
find . -not -path ./build -iregex '.*\.\(c\|h\|cpp\|hpp\|asm\)$' | xargs cat | wc -l
