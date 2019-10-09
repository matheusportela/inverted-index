// Document table data structure.
//
// A document table is a data structure that, given a document ID, is able to
// efficiently determine the URL of the original document web page and its size
// as in number of words.

#ifndef DOCUMENT_TABLE_HPP
#define DOCUMENT_TABLE_HPP

#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "document.hpp"

class DocumentTable {
  public:
    int size();
    void addDocument(std::shared_ptr<Document> document);
    std::string getDocumentURL(int documentID);
    int getDocumentSize(int documentID);

  private:
    std::map<int, std::tuple<std::string, int>> table;
};

#endif // DOCUMENT_TABLE_HPP
