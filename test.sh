#!/bin/bash
set -e

make all
echo "running tests..."

test() {
    local day=$1
    local diff="$(diff <(./target/"$day" inputs/"$day".txt) outputs/"$day".txt)"
    if [ "$diff" != "" ]; then
        echo "FAIL: " "$day"
        exit 1
    fi
}

for input in inputs/*; do
    file=$(basename -- "$input")
    day="${file%.*}"
    test "$day"
done

echo "done"
