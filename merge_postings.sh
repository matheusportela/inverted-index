#!/bin/bash

cd tmp
for filename in postings*.txt; do
    echo "Sorting $filename"
    sort $filename > sorted-$filename
done

echo "Merging to merged-postings.txt"
sort -m sorted-postings*.txt > merged-postings.txt

rm sorted-postings*.txt
