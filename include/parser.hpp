// Parse WET files containing text extracted from crawled web pages.

#ifndef PARSER_HPP
#define PARSER_HPP

#include <ctype.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "document.hpp"
#include "log.hpp"

class Parser {
  public:
    // Initialize parser with path containing WET file
    Parser(std::string path);

    // Read document from WET file. If parsed document is invalid, returns nullptr.
    std::shared_ptr<Document> parseDocument();

    // Check whether document has reached end-of-file
    bool isEOF();

  private:
    std::vector<std::string> parseDocumentLines();

    std::string parseLine();

    void setEOF();

    bool isDocumentBegin(std::string line);

    void processLine(std::string line);

    std::vector<std::string> parseDocumentHeaders(std::vector<std::string> lines);

    bool isLastHeader(std::string line);

    std::vector<std::string> parseDocumentContent(std::vector<std::string> lines);

    std::string parseDocumentURL(std::vector<std::string> headers);

    bool isURLHeader(std::string line);

    std::vector<std::string> parseDocumentTerms(std::vector<std::string> content);

    bool isValidCharacter(char c);

    std::vector<std::pair<std::string, int>> calculateFrequencies(std::vector<std::string> terms);

    bool isValidParsedDocument(std::string url);

    // Document to parse
    std::string path;

    // Document input file stream
    std::ifstream infile;

    // Document lines
    std::vector<std::string> lines;

    // Store whether file has reached EOF
    bool eof;
};

#endif // PARSER_HPP
