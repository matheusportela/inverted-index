/**
    Inverted list
*/

#ifndef INVERTED_LIST_HPP
#define INVERTED_LIST_HPP

#include <fstream>
#include <string>
#include <vector>

#include "compression.hpp"
#include "log.hpp"
#include "types.hpp"

// TODO: Find a more elegant way
#define MAX_DOC_ID 1000000000
#define BLOCK_SIZE 4 // postings

typedef std::pair<doc_id, int> posting_t;
typedef std::vector<posting_t> block_t;

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

    int write(std::ofstream& fd);
    void read(std::ifstream& fd);

    void open(std::string path, int offset);
    void close();
    doc_id nextGEQ(doc_id docID);

  private:
    int writeMetadata(std::ofstream& fd, std::vector<block_t> blocks);
    int writeBlocks(std::ofstream& fd, std::vector<block_t> blocks);
    int writeBlock(std::ofstream& fd, block_t block);
    int writeByteStream(std::ofstream& fd, std::vector<uint8_t> bytestream);
    std::vector<block_t> splitBlocks();
    int writeDocumentIDs(std::ofstream& fd, std::vector<doc_id> docIDs);
    int writeFrequencies(std::ofstream& fd, std::vector<int> frequencies);

    std::vector<uint8_t> compressDocumentIDs(std::vector<doc_id> docIDs);
    std::vector<uint8_t> compressFrequencies(std::vector<int> frequencies);

    void readMetadata(std::ifstream& fd);
    void readBlocks(std::ifstream& fd);
    void readBlockMetadata(std::ifstream& fd);
    void readBlock(std::ifstream& fd);
    void readDocumentIDs(std::ifstream& fd, uint32_t numBytes);
    void readFrequencies(std::ifstream& fd, uint32_t numBytes);
    std::vector<uint8_t> readByteStream(std::ifstream& fd, uint32_t numBytes);

    void skipBlock(std::ifstream& fd);
    bool shouldReadBlock(doc_id docID);
    bool hasReadAllDocuments();
    bool hasReadAllDocumentsInBlock();
    bool hasReadAllBlocks();

    list_p id;
    std::string term;

    // Attributes used when writing to file
    std::vector<doc_id> docIDs;
    std::vector<int> frequencies;
    std::vector<posting_t> postings;

    // Attributes used when reading from file
    uint32_t numDocs {0};
    uint32_t numBlocks {0};
    uint32_t currentDocID {0};
    int currentFrequency {0};

    int currentIndex {0};
    int numBlocksRead {0};

    std::ifstream indexFileStream;

    uint32_t blockLastDocID;
    uint32_t blockSize;
};

#endif // INVERTED_LIST_HPP
