#!/bin/bash

rm -rf tmp
mkdir tmp

cd build
./create_postings

cd ..
./merge_postings.sh

cd build
./create_index
