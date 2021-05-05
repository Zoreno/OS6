#!/bin/bash

find . -iregex '.*\.\(c\|h\|cpp\|hpp\)$' | xargs cat | wc -l
