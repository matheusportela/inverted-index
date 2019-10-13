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
$ ./create_postings
$ cd ..
$ ./merge.sh
$ cd -
$ ./create_index
```
