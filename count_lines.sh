#!/bin/bash

cloc --skip-uniqueness . | grep SUM: | sed -e 's/SUM://g' | jq -s add
