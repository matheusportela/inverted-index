/**
    Inverted list
*/

#ifndef INVERTED_LIST_HPP
#define INVERTED_LIST_HPP

#include <fstream>
#include <string>

#include "log.hpp"
#include "types.hpp"

// TODO: Find a more elegant way
#define INVERTED_LIST_END 1000000000

class InvertedList {
  public:
    InvertedList(std::string term);
    ~InvertedList();

    list_p getID();
    uint32_t getNumDocuments();
    doc_id getCurrentDocID();
    int getCurrentFrequency();

    void read(std::string indexFilePath, uint64_t listStart);
    doc_id nextGEQ(doc_id docID);

    void write();

    static list_p nextAvailableID;

  private:
    void readNumDocs(std::ifstream& fd);
    void readBlock(std::ifstream& fd);

    list_p id;
    std::string term;

    uint32_t numDocs;

    int currentIndex {0};
    uint32_t currentDocID {0};
    int currentFrequency {0};

    uint64_t indexOffset;
    uint32_t blockOffset {0};

    unsigned char* block;
};

#endif // INVERTED_LIST_HPP
