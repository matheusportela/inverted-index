// Lexicon data structure.
//
// A lexicon is a data structure that, given a term, is able to efficiently
// determine the start and end of the inverted index corresponding to this term
// as well as the number of documents containing the term. It also stores the
// mapping between terms and term IDs.

#ifndef LEXICON_HPP
#define LEXICON_HPP

#include <fstream>
#include <map>
#include <string>

#include "types.hpp"

class Lexicon {
  public:
    // Return number of terms in lexicon
    int size();

    // Check whether term exists in lexicon
    bool contains(std::string term);

    // Add term to lexicon and return term ID
    term_id addTerm(std::string term);

    // Add term to lexicon and return term ID if it does not exist. Otherwise,
    // simply return the existing term ID
    term_id addOrGetTerm(std::string term);

    term_id getID(std::string term);
    std::string getTerm(term_id id);

    void write(std::string path);

    static term_id nextAvailableID;

  private:
    std::map<std::string, term_id> stringToIDMap;
    std::map<term_id, std::string> idToStringMap;
};

#endif // LEXICON_HPP
