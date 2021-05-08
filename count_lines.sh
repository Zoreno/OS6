#!/bin/bash

echo -n "Files: "
find . -iregex '.*\.\(c\|h\|cpp\|hpp\)$' | wc -l

echo -n "Lines: "
find . -iregex '.*\.\(c\|h\|cpp\|hpp\)$' | xargs cat | wc -l
