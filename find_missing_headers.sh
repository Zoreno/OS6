#!/bin/bash -e

find . -name *.h -exec echo -n {} " " \; -exec head -n1 {} \; | grep -iv "/\*\*"
find . -name *.c -exec echo -n {} " " \; -exec head -n1 {} \; | grep -iv "/\*\*"
