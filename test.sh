#!/bin/bash
set -e

make all
echo "running tests..."

failed=0
test() {
    local input=$1
    local file=$(basename -- "$input")
    local output="outputs/$file"
    local day="${file%_*}"
    if [ ! -f "./target/$day" ]; then
        echo "day not found: $day"
        failed=1
    elif [ ! -f $output ]; then
        echo "output file not found: $output"
        failed=1
    elif [ "$(diff <(./target/"$day" $input) $output)" != "" ]; then
        echo "FAIL: $day ["$input"]"
        failed=1
    else echo "PASS: $day ["$input"]"
    fi
}

for input in inputs/*; do
    test "$input"
done

exit "$failed"
