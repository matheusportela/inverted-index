# Inverted Index
Didactic inverted index

## Downloading data

Downloading and uncompressing 10 WET files containing crawling data:

```bash
$ head -n 10 wet.paths | xargs -I {} wget "https://commoncrawl.s3.amazonaws.com/{}"
$ gzip -d CC-MAIN*
```

## Parsing data

This step processes WET documents and create intermediate postings.

```bash
$ python process_wet.py CC-MAIN-20190915052433-20190915074433-00000.warc.wet > parser/CC-MAIN-20190915052433-20190915074433-00000/intermediate.postings
```
