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
#include "lexicon.hpp"
#include "log.hpp"
#include "types.hpp"

class InvertedIndex {
  public:
    void buildFromIntermediatePostings(std::string inputPath, std::string outputPath, Lexicon& lexicon);

    std::vector<std::pair<doc_id, int>> getInvertedList(std::string path, int listStart);

  private:
    std::tuple<std::string, doc_id, int> readPosting();
    // std::tuple<term_id, doc_id, int> readPosting();
    void processPosting(std::tuple<std::string, doc_id, int> posting, Lexicon& lexicon);
    // void processPosting(std::tuple<term_id, doc_id, int> posting, Lexicon& lexicon);
    bool isNewTerm(std::string termID);
    // bool isNewTerm(term_id termID);
    void createInvertedList(std::string termID);
    // void createInvertedList(term_id termID);
    void flushInvertedList(Lexicon& lexicon);

    std::ifstream input;
    std::ofstream output;

    bool isFirstTerm {true};

    std::string currentTerm;
    // term_id currentTermID {0};
    std::vector<doc_id> currentDocIDs;
    std::vector<int> currentFrequencies;

    int currentIndexOffset {0};
};

#endif // INVERTED_INDEX_HPP
