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
#define MAX_DOC_ID 1000000000

class InvertedList {
  public:
    static list_p nextAvailableID;

    InvertedList(std::string term);

    list_p getID();
    std::string getTerm();
    uint32_t getNumDocuments();
    doc_id getCurrentDocID();
    int getCurrentFrequency();

    void addPosting(doc_id docID, int frequency);

    int write(std::ofstream& fd);
    void read(std::ifstream& fd);

    doc_id nextGEQ(doc_id docID);

  private:
    int writeNumberOfDocs(std::ofstream& fd);
    int writeDocumentIDs(std::ofstream& fd);
    int writeFrequencies(std::ofstream& fd);

    void readNumDocs(std::ifstream& fd);
    void readDocumentIDs(std::ifstream& fd);
    void readFrequencies(std::ifstream& fd);

    uint32_t byteToUInt32(uint8_t* bytes, int addr);

    list_p id;
    std::string term;

    // Attributes used when writing to file
    std::vector<doc_id> docIDs;
    std::vector<int> frequencies;

    // Attributes used when reading from file
    uint32_t numDocs {0};
    int currentIndex {0};
    uint32_t currentDocID {0};
    int currentFrequency {0};
};

#endif // INVERTED_LIST_HPP
