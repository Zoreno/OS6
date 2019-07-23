#!/bin/bash

cloc --skip-uniqueness --exclude-dir=toolchain . | grep SUM: | sed -e 's/SUM://g' | jq -s add
