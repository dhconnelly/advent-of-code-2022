#!/bin/bash
set -e

DAYS=day1

make all

test() {
    local day=$1
    local diff="$(diff <(./target/"$day" inputs/"$day".txt) outputs/"$day".txt)"
    if [ "$diff" != "" ]; then
        echo "FAIL: " "$day"
        exit 1
    fi
}

for day in "$DAYS"; do
    test "$day"
done
