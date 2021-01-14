#!/bin/bash

count_file=count.txt

rm -f $count_file

for dir in kernel libc libk libsimd libstdc++ libsupc++ libunwind apps tests unittest
do
    echo -n $dir
    echo -n " "
    cloc --skip-uniqueness --exclude-dir=toolchain $dir | grep SUM: | sed -e 's/SUM://g' | jq -s add | tee -a $count_file
done

echo -n "total "
awk '{ sum += $1 } END { print sum }' $count_file

rm -f $count_file