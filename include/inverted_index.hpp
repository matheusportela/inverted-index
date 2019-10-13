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
    void add(std::shared_ptr<Document> document);
    std::vector<std::pair<doc_id, int>> search(term_id termID);

    void buildFromIntermediatePostings(std::string inputPath, std::string outputPath, Lexicon& lexicon);

    std::vector<doc_id> getInvertedList(std::string path, int listStart);

  private:
    std::tuple<term_id, doc_id, int> readPosting();
    void processPosting(std::tuple<term_id, doc_id, int> posting, Lexicon& lexicon);
    bool isNewTerm(term_id termID);
    void createInvertedList(term_id termID);
    void flushInvertedList(Lexicon& lexicon);

    std::map<term_id, std::vector<std::pair<doc_id, int>>> index;

    std::ifstream input;
    std::ofstream output;

    term_id currentTermID {0};
    std::vector<doc_id> currentDocIDs;
    std::vector<int> currentFrequencies;

    int currentIndexOffset {0};
};

#endif // INVERTED_INDEX_HPP
