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

    std::tuple<int, int, int> getMetadata(std::string term);
    void addTermMetadata(std::string term, int invertedListStart, int invertedListEnd, int numDocs);

    void writeIntermediate(std::string path);
    void readIntermediate(std::string path);

    void write(std::string path);
    void read(std::string path);

    static term_id nextAvailableID;

  private:
    // Used for intermediate lexicon
    std::unordered_map<std::string, term_id> stringToIDMap;
    std::unordered_map<term_id, std::string> idToStringMap;

    // Used for final lexicon
    std::unordered_map<std::string, std::tuple<int, int, int>> stringToMetadataMap;
};

#endif // LEXICON_HPP
