// Document data structure.
//
// A document represents a crawled web page, containing its URL and parsed
// terms. Each document has a unique monotonically-increasing ID.
//
// Document is not thread-safe.

#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <map>
#include <utility>
#include <string>
#include <vector>

#include "types.hpp"

class Document {
  public:
    Document(std::string url, std::vector<std::pair<term_id, int>> frequencies);

    doc_id getID();
    std::string getURL();
    std::vector<std::pair<term_id, int>> getFrequencies();
    int getSize();

    static doc_id nextAvailableID;

  private:
    const doc_id id;
    const std::string url;
    const std::vector<std::pair<term_id, int>> frequencies;
};

#endif // DOCUMENT_HPP
