// Lexicon data structure.
//
// A lexicon is a data structure that, given a term, is able to efficiently
// determine the start and end of the inverted index corresponding to this term
// as well as the number of documents containing the term. It also stores the
// mapping between terms and term IDs.

#ifndef LEXICON_HPP
#define LEXICON_HPP

#include <fstream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "log.hpp"

class Lexicon {
  public:
    // Return number of terms in lexicon
    int size();

    std::tuple<int, int, int> getMetadata(std::string term);
    void addTermMetadata(std::string term, int invertedListStart, int invertedListEnd, int numDocs);

    void write(std::string path);
    void read(std::string path);

  private:
    std::unordered_map<std::string, std::tuple<int, int, int>> stringToMetadataMap;
};

#endif // LEXICON_HPP
