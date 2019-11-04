/**
    Parse crawled web pages in WET format and write postings and document table files.

    DocumentParser reads a WET file containing crawled web pages content, parse
    documents from it (doc_id, url, size, [term, freq]), and write postings file
    and document table file.

    Posting is a tuple of (term, doc_id, freq), where:
      - term: Alphanumeric string extracted from web page
      - doc_id: Document ID number (e.g. 0, 1, 2...) from which this term was extracted
      - freq: Number of times this term appeared in the document

    Posting file contains one posting tuple (term, doc_id, freq) per line,
    separated by spaces. Terms originating from one document will be adjacent
    but unordered in the postings file. All postings extracted from a single WET
    file, which belong to different documents, will be written to the same
    postings file.

    Example: postings-0.txt
        www 0 3
        icp 0 3
        com 0 3
        solutions 1 7
        gmt 1 2
        tip 1 1
        icp 2 1
        contact 2 2
            ...

    Document table is a mapping of doc_id -> (url, size), where:
      - doc_id: Document ID number (e.g. 0, 1, 2...) from which this term was extracted
      - url: URL where this document was crawled from
      - size: Number of terms in the document

    Document table file contains one entry (url, size) per file. doc_id is not
    explicitly written to the file because it can be inferred from the line
    number. For instance, the first entry has doc_id = 0, the second doc_id = 1,
    and so on. All documents extracted across multiple WET files will be written
    to the same document table file in their respective order of appearance.

    Example: document-table.txt
        http://www.nyu.edu 350
        http://www.google.com 90
        http://www.unb.br 125
                ...
*/

#ifndef DOCUMENT_PARSER_HPP
#define DOCUMENT_PARSER_HPP

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "log.hpp"
#include "types.hpp"
#include "wet_parser.hpp"

class DocumentParser {
  public:
    // @param inputDir - Directory containing only WET files
    // @param outputDir - Directory to write postings and document table files
    DocumentParser(std::string inputDir, std::string outputDir);
    ~DocumentParser();

    // Parse WET files from input directory and writing postings and document table files to output directory
    void parse();

  private:
    // Get WET file paths from directory
    // @param directoryPath - Directory containing only WET files
    // @return Vector containing file paths as string
    std::vector<std::string> getFilePaths(std::string directoryPath);

    // Generate postings file output path
    // @param outputDir - Directory to write postings file
    // @param postingNumber - Unique number identifying current posting file
    // @return File path as string
    std::string generatePostingPath(std::string outputDir, int postingNumber);

    // Parse WET file, writing to postings and document table files
    // @param wetFilePath - WET file path to be read
    // @param postingFilePath - Postings file path to be written
    void parseWETFile(std::string wetFilePath, std::string postingFilePath);

    // Append posting entries to postings file
    // @param fd - Posting file descriptor
    // @param docID - Uniquely generated document ID
    // @param frequencies - Vector of (term, frequency)
    void appendPostingEntries(std::ofstream& fd, doc_id docID, std::vector<std::pair<std::string, int>> frequencies);

    // Append posting entry to postings file
    // @param fd - Posting file descriptor
    // @param docID - Uniquely generated document ID
    // @param term - Term string
    // @param frequency - Number of times term appeared in document `docID`
    void appendPostingEntry(std::ofstream& fd, doc_id docID, std::string term, int frequency);

    // Append document table entry to document table file
    // @param fd - Document table file descriptor
    // @param url - Web page URL
    // @param num_terms - Number of terms in document
    // @param document_begin - Byte position of document in WET file
    // @param document_length - Number of characters in document
    // @param path - File containing document
    void appendDocumentTableEntry(std::ofstream& fd, std::string url, int num_terms, uint64_t document_begin, uint64_t document_length, std::string path);

    // Generate unique, monotonically increasing document ID starting from 0
    // @return Unique document ID
    doc_id generateDocumentID();

    const std::string inputDir;
    const std::string outputDir;
    std::ofstream documentTableFileStream;
    doc_id nextAvailableDocumentID {0};
};

#endif // DOCUMENT_PARSER_HPP
