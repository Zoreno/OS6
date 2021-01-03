#!/bin/bash -e

error_index=0
run_count=100
log_dir=log

mkdir -p $log_dir

do_run ()
{
    make run > $log_dir/log.txt
    if grep -qi "error" $log_dir/log.txt; then
        echo "ERROR"
        cp $log_dir/log.txt $log_dir/log_$error_index.txt
        error_index=$((error_index+1))
    else
        echo "OK"
    fi
}

for i in $(seq 1 $run_count)
do
    echo -n "Run $i/$run_count: "
    do_run
done
