// Inverted index data structure.
//
// An inverted index is a data structure that, given a term, is able to
// efficiently determine all indexed documents containing the aforementioned
// term.

#ifndef INVERTED_INDEX_HPP
#define INVERTED_INDEX_HPP

#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "document.hpp"

class InvertedIndex {
  public:
    void add(std::shared_ptr<Document> document);
    std::vector<std::pair<int, int>> search(std::string term);

  private:
    std::map<std::string, std::vector<std::pair<int, int>>> index;
};

#endif // INVERTED_INDEX_HPP
