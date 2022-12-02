#!/bin/bash
set -e

make all
echo "running tests..."

failed=0
test() {
    local day=$1
    local diff="$(diff <(./target/"$day" inputs/"$day".txt) outputs/"$day".txt)"
    if [ "$diff" != "" ]; then
        echo "FAIL: " "$day"
        failed=1
    else echo "PASS: " "$day"
    fi
}

for input in inputs/*; do
    file=$(basename -- "$input")
    day="${file%.*}"
    test "$day"
done

exit "$failed"
