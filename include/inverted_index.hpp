// Inverted index data structure.
//
// An inverted index is a data structure that, given a term, is able to
// efficiently determine all indexed documents containing the aforementioned
// term.

#ifndef INVERTED_INDEX_HPP
#define INVERTED_INDEX_HPP

#include <fstream>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "document.hpp"
#include "types.hpp"
#include "lexicon.hpp"

class InvertedIndex {
  public:
    void add(std::shared_ptr<Document> document);
    std::vector<std::pair<doc_id, int>> search(term_id termID);

    void write_intermediate_postings(std::string path, Lexicon lexicon);

  private:
    std::map<term_id, std::vector<std::pair<doc_id, int>>> index;
};

#endif // INVERTED_INDEX_HPP
