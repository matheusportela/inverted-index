/**
    Parse WET files and create postings and document table files.

    Usage:
      $ ./parse
*/

#include "document_parser.hpp"

int main() {
    LOG_SET_INFO();

    const std::string inputDir = "../data/common-crawl";
    const std::string outputDir = "../tmp";

    LOG_I("Parsing WET files");

    DocumentParser documentParser(inputDir, outputDir);
    documentParser.parse();

    LOG_I("Parsed WET files");

    return 0;
}
