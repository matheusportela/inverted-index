/**
    Inverted list data structure.

    An inverted list is composed of blocks of at most 128 document IDs and
    their respective frequencies. Each block contains the following data:

    - lastDocID (uint32_t): Number of the last document ID in the block
    - blockSize (uint32_t): Size of the remaining block data, in bytes
    - docIDsSize (uint32_t): Size of docIDs, in bytes
    - docIDs (uint8_t[]): Compressed sorted vector of document IDs differences
        stored in the block
    - freqsSize (uint32_t): Size of freqs, in bytes
    - freqs (uint8_t[]): Compressed vector of frequencies stored in the block in
        the same order of docIDs

    The following diagram shows an inverted list structure in disk and the
    associated size of each element.

    Inverted List:
    | 32 bits |  32 bits  | variable |
    | numDocs | numBlocks |  blocks  |

    Block:
    |  32 bits  |  32 bits  |   32 bits  | variable |  32 bits  | variable |
    | lastDocID | blockSize | docIDsSize |  docIDs  | freqsSize |  freqs   |

    Document IDs are stored as differences. A docID that will be stored in
    position N in the vector will be stored as docID[N] - docID[N-1]. Likewise,
    when retrieving the docID from the list, the docID in position N is the sum
    of all docIDs before it: docID = docID[0] + docID[1] + ... + doc[N-1].

    For instance, given the following postings:
        icp 0 3
        icp 2 1
        icp 3 1
        icp 6 2
        icp 7 5
        icp 8 1
    the uncompressed inverted list with 4 postings per block, where each
    integer is stored in 4 bytes, will be:
        5 36 6 40 16 0 2 1 3 16 3 1 1 2 8 24 8 7 1 8 5 1
    where:
        - 5: number of documents
        - 3: number of blocks
        - 6 40 16 0 2 1 3 16 3 1 1 2: block 1, where:
            - 6: last document ID
            - 40: block size in bytes
            - 16: document IDs size in bytes
            - 0 2 1 3: document IDs stored as differences
            - 16: frequencies size in bytes
            - 3 1 1 2: frequencies
        - 8 24 8 7 1 8 5 1: block 2, where:
            - 8: last document ID
            - 24: block size in bytes
            - 8: document IDs size in bytes
            - 7 1: document IDs stored as differences
            - 8: frequencies size in bytes
            - 5 1: frequencies
*/

#ifndef INVERTED_LIST_HPP
#define INVERTED_LIST_HPP

#include <fstream>
#include <queue>
#include <string>
#include <vector>

#include "compression.hpp"
#include "log.hpp"
#include "types.hpp"

// Maximum number of indexed document
#define MAX_DOC_ID 1000000000

// Maximum number of postings per inverted list block
#define BLOCK_SIZE 128

// Alias for postings
typedef std::pair<doc_id, int> posting_t;

// Alias for inverted list block
typedef std::vector<posting_t> block_t;

class InvertedList {
  public:
    static list_p nextAvailableID;

    // Create new inverted list for the given term
    // @param term - Term associated with the inverted list
    InvertedList(std::string term);
    ~InvertedList();

    // @return Inverted list unique ID
    list_p getID();

    // @return Term associated with this inverted list
    std::string getTerm();

    // @return Number of documents indexed in this inverted list
    uint32_t getNumDocuments();

    // @return Current document ID when reading list through nextGEQ
    doc_id getCurrentDocID();

    // @return Current frequency when reading list through nextGEQ
    int getCurrentFrequency();

    // Add posting to inverted list
    // @param docID - Document unique ID
    // @param frequency - Number of times the term appeared in the document
    void addPosting(doc_id docID, int frequency);

    // Write inverted list to file in binary format
    // @param fd - File descriptor where inverted list will be written. fd must
    //             be already in the correct position where the list will start.
    // @return Number of bytes written
    int write(std::ofstream& fd);

    // Read inverted list from file in binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position where the list starts.
    void read(std::ifstream& fd);

    // Open inverted list
    // @param path - Path of the inverted index file
    // @param offset - Byte offset where the inverted list starts in the file
    void open(std::string path, uint64_t offset);

    // Close inverted list
    void close();

    // Find the next document ID greater or equal to docID in an open inverted
    // list. If docID is less than the current docID for the list, simply
    // returns the current docID.
    // @param docID - document ID number
    // @return document ID greater than or equal to docID
    doc_id nextGEQ(doc_id docID);

  private:
    // Write number of documents and number of blocks to file in binary format
    // @param fd - File descriptor where inverted list will be written. fd must
    //             be already in the correct position
    // @param blocks - Vector of blocks to be written to the file
    // @return Number of bytes written
    int writeMetadata(std::ofstream& fd, std::vector<block_t> blocks);

    // Write blocks to file in binary format
    // @param fd - File descriptor where inverted list will be written. fd must
    //             be already in the correct position
    // @param blocks - Vector of blocks to be written to the file
    // @return Number of bytes written
    int writeBlocks(std::ofstream& fd, std::vector<block_t> blocks);

    // Write block to file in binary format
    // @param fd - File descriptor where inverted list will be written. fd must
    //             be already in the correct position
    // @param block - Block to be written to the file
    // @return Number of bytes written
    int writeBlock(std::ofstream& fd, block_t block);

    // Write vector of bytes to file in binary format
    // @param fd - File descriptor where inverted list will be written. fd must
    //             be already in the correct position
    // @param bytestream - Vector of bytes to be written to the file
    // @return Number of bytes written
    int writeByteStream(std::ofstream& fd, std::vector<uint8_t> bytestream);

    // Split the inverted list postings into blocks of at most 128 postings
    // @return Vector of blocks containing at most 128 postings
    std::vector<block_t> splitBlocks();

    // Compress document IDs in difference format
    // @return Vector of compressed document IDs
    std::vector<uint8_t> compressDocumentIDs(std::vector<doc_id> docIDs);

    // Compress frequencies
    // @return Vector of compressed frequencies
    std::vector<uint8_t> compressFrequencies(std::vector<int> frequencies);

    // Read number of documents and number of blocks from file in binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    void readMetadata(std::ifstream& fd);

    // Read block metadata from file in binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    void readBlockMetadata(std::ifstream& fd);

    // Read block data (document IDs and frequencies) from file in binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    void readBlock(std::ifstream& fd);

    // Read document IDs from file in compressed binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    // @param numBytes - Number of bytes containing the compressed
    //                   representation of document IDs
    void readDocumentIDs(std::ifstream& fd, uint32_t numBytes);

    // Read frequencies from file in compressed binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    // @param numBytes - Number of bytes containing the compressed
    //                   representation of frequencies
    void readFrequencies(std::ifstream& fd, uint32_t numBytes);

    // Read uint32_t number from file in binary little-endian format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    // @return uint32_t number
    uint32_t readUInt32(std::ifstream& fd);

    // Read a vector of bytes from file in binary format
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    // @param numBytes - Number of bytes to be read from file
    // @return Vector of uint8_t bytes
    std::vector<uint8_t> readByteStream(std::ifstream& fd, uint32_t numBytes);

    // Skip next inverted list block in the inverted index file
    // @param fd - File descriptor where inverted list is written. fd must be
    //             already in the correct position
    void skipBlock(std::ifstream& fd);

    // Defines whether the block data must be read
    // @param docID - Document ID
    // @return true if docID is less than the block lastDocID, false otherwise
    bool shouldReadBlock(doc_id docID);

    // @return true if all postings in the inverted list have been read or
    // skipped
    bool hasReadAllDocuments();

    // @return true if all postings in the current block have been read
    bool hasReadAllDocumentsInBlock();

    // @return true if all blocks in the inverted list have been read of skipped
    bool hasReadAllBlocks();

    // Inverted list unique ID
    list_p id;

    // Inverted list associated term
    std::string term;

    // Number of documents in the inverted list
    uint32_t numDocs {0};

    // Attributes used when writing to file
    std::vector<doc_id> docIDs; // All document IDs in the inverted list
    std::vector<int> frequencies; // All frequencies in the inverted list
    std::vector<posting_t> postings; // All postings in the inverted list

    // Attributes used when reading from file
    std::queue<doc_id> blockDocIDs; // All document IDs in the current block
    std::queue<int> blockFrequencies; // All frequencies in the current block
    uint32_t numBlocks {0}; // Number of blocks in the inverted list
    int numBlocksRead {0}; // Number of blocks read of skipped from the inverted list
    uint32_t currentDocID {0}; // Last document ID read from the inverted list
    int currentFrequency {0}; // Last frequency read from the inverted list
    uint32_t blockLastDocID; // Last document ID in the current block
    uint32_t blockSize; // Size of the current block, in bytes
    std::ifstream indexFileStream; // Inverted index file descriptor
};

#endif // INVERTED_LIST_HPP
