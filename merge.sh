#!/bin/bash

cd build
for filename in index*.txt; do
    echo "Sorting $filename"
    sort $filename > sorted-$filename
done

echo "Merging"
sort -m sorted-index*.txt > merged-index.txt

rm sorted-index*.txt
