// Document table data structure.
//
// A document table is a data structure that, given a document ID, is able to
// efficiently determine the URL of the original document web page and its size
// as in number of terms.

#ifndef DOCUMENT_TABLE_HPP
#define DOCUMENT_TABLE_HPP

#include <fstream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "document.hpp"

class DocumentTable {
  public:
    int size();
    void addDocument(std::shared_ptr<Document> document);
    std::string getDocumentURL(doc_id documentID);
    int getDocumentSize(doc_id documentID);

    void write(std::string path);
    void read(std::string path);

  private:
    // Maps doc_id to (URL, page size)
    std::vector<std::tuple<std::string, int>> documents;
};

#endif // DOCUMENT_TABLE_HPP
