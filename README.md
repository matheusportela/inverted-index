# Inverted Index
Didactic inverted index

## Downloading data

Download and uncompress 10 WET files containing crawling data:

```bash
$ head -n 10 wet.paths | xargs -I {} wget "https://commoncrawl.s3.amazonaws.com/{}"
$ gzip -d CC-MAIN*
```

## Parsing data

Parse WET document and save parsed words to a file.

```bash
$ python process_wet.py CC-MAIN-20190915052433-20190915074433-00000.warc.wet > parser.00000
```

Each line contains a URL and parsed words, separated by tabs. Example:

```
https://www.nyu.edu New York    University
https://www.unb.br  Universidade    de  Brasília
```