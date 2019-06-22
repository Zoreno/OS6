#!/bin/bash

cloc . | grep SUM: | sed -e 's/SUM://g' | jq -s add