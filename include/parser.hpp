// Parse WET files containing text extracted from crawled web pages.

#ifndef PARSER_HPP
#define PARSER_HPP

#include <ctype.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "log.hpp"

class Parser {
  public:
    // Initialize parser with path containing WET file
    Parser(std::string path);

    // Read document from WET file and return pair (url, words)
    std::pair<std::string, std::vector<std::string>> getDocument();

    // Check whether document has reached end-of-file
    bool isEOF();

  private:
    std::vector<std::string> getDocumentLines();

    std::string getLine();

    void setEOF();

    bool isDocumentBegin(std::string line);

    void processLine(std::string line);

    std::vector<std::string> getDocumentHeaders(std::vector<std::string> lines);

    bool isLastHeader(std::string line);

    std::vector<std::string> getDocumentContent(std::vector<std::string> lines);

    std::string getDocumentURL(std::vector<std::string> headers);

    bool isURLHeader(std::string line);

    std::vector<std::string> getDocumentWords(std::vector<std::string> content);

    bool isValidCharacter(char c);

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
