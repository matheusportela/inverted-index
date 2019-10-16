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

#include "lexicon.hpp"
#include "log.hpp"
#include "types.hpp"

class InvertedIndex {
  public:
    InvertedIndex(std::string path);

    void index();

    std::vector<std::pair<doc_id, int>> getInvertedList(std::string path, int listStart);

  private:
    std::tuple<std::string, doc_id, int> readPosting();
    void processPosting(std::tuple<std::string, doc_id, int> posting);
    bool isNewTerm(std::string termID);
    void createInvertedList(std::string termID);
    void flushInvertedList();
    void writeNumberOfDocs(uint32_t numDocs);
    void writeDocumentIDs();
    void writeFrequencies();
    void write(char* addr, unsigned int size);

    void readNumberOfDocs();
    void readDocumentIDs();
    void readFrequencies();

    std::ifstream postingsFileStream;
    std::ofstream indexFileStream;

    std::string postingsPath;
    std::string indexPath;
    std::string lexiconPath;

    std::string currentTerm;
    std::vector<doc_id> currentDocIDs;
    std::vector<int> currentFrequencies;

    int currentIndexOffset {0};

    Lexicon lexicon;
};

#endif // INVERTED_INDEX_HPP
