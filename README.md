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
$ mkdir -p data/common-crawl
$ cd data/
$ wget https://commoncrawl.s3.amazonaws.com/crawl-data/CC-MAIN-2019-39/wet.paths.gz
$ gzip -d wet.paths.gz
$ head -n 10 wet.paths | xargs -I {} wget "https://commoncrawl.s3.amazonaws.com/{}"
$ gzip -d CC-MAIN*.gz
$ mv CC-MAIN*.wet common-crawl
```

## Indexing data

Index all WET documents in `data/common-crawl`.

```bash
$ ./create_inverted_index.sh
```

## Searching

Search for single-word terms in inverted index.

```bash
$ ./search.sh
```
