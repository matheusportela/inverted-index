/**
    Parse WET files containing text extracted from crawled web pages.

    WET files contain text data extracted from web pages in WARC format. A
    single WET file contains approximately 40,000 WARC files such as the
    following:
        WARC/1.0
        WARC-Type: warcinfo
        WARC-Target-URI: http://000754.com/Article/Article.aspx?Id=6&menuId=3&cid=2
        WARC-Date: 2019-09-23T21:43:59Z
        WARC-Filename: CC-MAIN-20190915052433-20190915074433-00000.warc.wet.gz
        WARC-Record-ID: <urn:uuid:b1a6e537-753b-4457-a039-15d0c9c14834>
        Content-Type: application/warc-fields
        Content-Length: 373

        Software-Info: ia-web-commons.1.1.9-SNAPSHOT-20190909091644
        Extracted-Date: Mon, 23 Sep 2019 21:43:59 GMT
        robots: checked via crawler-commons 1.1-SNAPSHOT (https://github.com/crawler-commons/crawler-commons)
        isPartOf: CC-MAIN-2019-39
        operator: Common Crawl Admin (info@commoncrawl.org)
        description: Wide crawl of the web for September 2019
        publisher: Common Crawl

    This parser extracts the URL from the "WARC-Target-URI" header and text data
    after the "Content-Length" header.

    Only alphanumerical ASCII characters are valid. Terms are considered valid
    when their size is at least 1 and at most 40 characters, contain at least 1
    letter.

    The parser count the frequency for each term in a given WARC document.
*/

#ifndef WET_PARSER_HPP
#define WET_PARSER_HPP

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "log.hpp"

#define MAX_TERM_SIZE 40

class WETParser {
  public:
    // @param path - WET file path
    WETParser(std::string path);

    // Parse document from WET file
    // @return URL and vector of (term, freq) for all terms in the document
    std::pair<std::string, std::vector<std::pair<std::string, int>>> parseDocument();

    // Check whether parser has reached end-of-file
    bool isEOF();

  private:
    // Parse all text lines for one document
    // @return Vector of lines
    std::vector<std::string> parseDocumentLines();

    // Get one line, delimited by new line
    // @return Line as string
    std::string parseLine();

    // Set end-of-file flag to true
    void setEOF();

    // Check whether line is the beginning of a new WARC document
    // @param line - Line from WET file
    // @return True if it is the header of a new WARC document, false otherwise
    bool isDocumentBegin(std::string line);

    // Get all headers in WARC format for one document
    // @param lines - Lines for a WARC document
    // @return Vector of lines that correspond to headers
    std::vector<std::string> parseDocumentHeaders(std::vector<std::string> lines);

    // Check whether the line is the last header in the WARC headers
    // @param line - Line from WARC document headers
    // @return True if it is "Content-Length" header, false otherwise
    bool isLastHeader(std::string line);

    // Get all content files in WARC format for one document
    // @param lines - Lines for a WARC document
    // @return Vector of lines that correspond to content
    std::vector<std::string> parseDocumentContent(std::vector<std::string> lines);

    // Get document URL from header lines
    // @param headers - Header lines from WARC document
    // @return Extracted URL if it exists or empty string otherwise
    std::string parseDocumentURL(std::vector<std::string> headers);

    // Check whether line is URL header in WARC format
    // @param line - Header line from WARC document
    // @return True if it is "WARC-Target-URI" header, false otherwise
    bool isURLHeader(std::string line);

    // Parse document terms from content lines
    // @param content - Lines from WARC document
    // @return Vector of valid words
    std::vector<std::string> parseDocumentTerms(std::vector<std::string> content);

    // Check whether character is valid
    // @param c - Character
    // @return True if valid, false otherwise
    bool isValidCharacter(char c);

    // Check whether term is valid
    // @param term - Term as string
    // @return True if valid, false otherwise
    bool isValidTerm(std::string term);

    // Check whether term is not digits only
    // @param term - Term as string
    // @return True if term has at least 1 letter, false otherwise
    bool isTermNotOnlyDigits(std::string term);

    // Calculate the number of times each term appear in a document
    // @param terms - Unordered vector of terms from a document
    // @return Vector of pairs (term, freq)
    std::vector<std::pair<std::string, int>> calculateFrequencies(std::vector<std::string> terms);

    // Document to parse
    std::string path;

    // Document input file stream
    std::ifstream infile;

    // Document lines
    std::vector<std::string> lines;

    // Store whether file has reached EOF
    bool eof;
};

#endif // WET_PARSER_HPP
