// Lexicon data structure.
//
// A lexicon is a data structure that, given a term, is able to efficiently
// determine the start and end of the inverted index corresponding to this term
// as well as the number of documents containing the term. It also stores the
// mapping between terms and term IDs.

#ifndef LEXICON_HPP
#define LEXICON_HPP

#include <map>
#include <string>

typedef int term_id;

class Lexicon {
  public:
    // Add term to lexicon and return term ID
    term_id addTerm(std::string term);

    term_id getID(std::string term);
    std::string getTerm(term_id id);

    static term_id nextAvailableID;

  private:
    std::map<std::string, term_id> stringToIDMap;
    std::map<term_id, std::string> idToStringMap;
};

#endif // LEXICON_HPP
