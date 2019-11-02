/**
    Inverted list
*/

#ifndef INVERTED_LIST_HPP
#define INVERTED_LIST_HPP

#include <fstream>
#include <string>
#include <vector>

#include "log.hpp"
#include "types.hpp"

// TODO: Find a more elegant way
#define INVERTED_LIST_END 1000000000

class InvertedList {
  public:
    static list_p nextAvailableID;

    InvertedList(std::string term);
    ~InvertedList();

    list_p getID();
    std::string getTerm();
    uint32_t getNumDocuments();
    doc_id getCurrentDocID();
    int getCurrentFrequency();

    void addPosting(doc_id docID, int frequency);

    void read(std::ifstream& fd);
    doc_id nextGEQ(doc_id docID);

    int write(std::ofstream& fd);
    int writeNumberOfDocs(std::ofstream& fd);
    int writeDocumentIDs(std::ofstream& fd);
    int writeFrequencies(std::ofstream& fd);

  private:
    void readNumDocs(std::ifstream& fd);
    void readBlock(std::ifstream& fd);

    list_p id;
    std::string term;
    std::vector<doc_id> docIDs;
    std::vector<int> frequencies;

    // Reading attributes
    uint32_t numDocs {0};
    int currentIndex {0};
    uint32_t currentDocID {0};
    int currentFrequency {0};
    uint32_t blockOffset {0};
    unsigned char* block {NULL};
};

#endif // INVERTED_LIST_HPP
