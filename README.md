# Inverted Index

Didactic inverted index

## Requirements

- C++11
- CMake
- GNU Make

## Compiling

```bash
$ mkdir build && cd build && cmake .. && make
```

## Downloading data

Download and uncompress 10 WET files containing crawling data:

```bash
$ mkdir data
$ cd data/
$ wget https://commoncrawl.s3.amazonaws.com/crawl-data/CC-MAIN-2019-39/wet.paths.gz
$ gzip -d wet.paths.gz
$ head -n 10 wet.paths | xargs -I {} wget "https://commoncrawl.s3.amazonaws.com/{}"
$ gzip -d CC-MAIN*.gz
```

## Running

Parse WET document and save parsed words to a file.

```bash
$ cd build
$ make
$ ./inverted_index
```

## Sorting and Merging
```bash
# sorting each file independently
$ sort build/index0.txt > build/sort-index0.txt
$ sort build/index1.txt > build/sort-index1.txt
# merging files into one
$ sort -m build/sort-index*.txt > build/merged-index.txt
```
