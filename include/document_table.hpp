/**
    Document table data structure.

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

#ifndef DOCUMENT_TABLE_HPP
#define DOCUMENT_TABLE_HPP

#include <fstream>
#include <string>
#include <tuple>
#include <vector>

#include "types.hpp"

class DocumentTable {
  public:
    // @param dir - Directory containing document table file
    DocumentTable(std::string dir);

    // Return number of documents in the table
    int size();

    // Return document URL from table given document ID
    std::string getDocumentURL(doc_id documentID);

    // Return document size from table given document ID
    int getDocumentSize(doc_id documentID);

    // Return average document size when table is loaded
    float getAverageDocumentSize();

    // Load document table from file
    void load();

  private:
    std::string documentTableFilePath;

    // Maps doc_id to (URL, page size)
    std::vector<std::tuple<std::string, int>> documents;

    float average_document_size;
};

#endif // DOCUMENT_TABLE_HPP
