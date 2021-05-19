#!/bin/bash

echo -n "Files: "
find . -not -path ./build -iregex '.*\.\(c\|h\|cpp\|hpp\)$' | wc -l

echo -n "Lines: "
find . -not -path ./build -iregex '.*\.\(c\|h\|cpp\|hpp\)$' | xargs cat | wc -l
