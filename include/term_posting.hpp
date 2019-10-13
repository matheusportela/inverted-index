// Inverted index data structure.
//
// An inverted index is a data structure that, given a term, is able to
// efficiently determine all indexed documents containing the aforementioned
// term.

#ifndef TERM_POSTING_HPP
#define TERM_POSTING_HPP

#include <fstream>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "document.hpp"
#include "lexicon.hpp"
#include "log.hpp"
#include "types.hpp"

class TermPosting {
  public:
    TermPosting(term_id termID, std::vector<std::pair<doc_id, int>> frequencies);
    void write(std::string path, Lexicon lexicon);
    void read_intermediate_postings(std::string path);

  private:
    std::map<term_id, std::vector<std::pair<doc_id, int>>> index;
};

#endif // TERM_POSTING_HPP
