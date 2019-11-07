/**
    Inverted index data structure.

    An inverted index is a data structure that, given a term, is able to
    efficiently determine all indexed documents containing the aforementioned
    term. It is a collection of inverted lists, one for each indexed term.

    Usually, the inverted index is too large to fit main memory, so we store the
    inverted lists on a file. In order to be able to efficiently fetch inverted
    lists from disk, the inverted index uses a lexicon (which fits in main
    memory) - a data structure that can efficiently determine the inverted
    list  address in the index file for any given a term. Having this
    information, the inverted index seeks this position and read binary data
    sequentially.

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

    Besides of its blocks, the inverted list also stores two pieces of metadata
    at its beginning:

    - numDocs (uint32_t): Number of documents in the inverted list
    - numBlocks (uint32_t): Number of blocks in the inverted list

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

    The inverted index file contains consecutive inverted lists, written in binary format.

    Inverted index indexes postings from a single file with all term postings
    being in consecutive lines. This should be done beforehand by Unix
    merge-sort for better I/O efficiency.
*/

#ifndef INVERTED_INDEX_HPP
#define INVERTED_INDEX_HPP

#include <fstream>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "inverted_list.hpp"
#include "lexicon.hpp"
#include "log.hpp"
#include "types.hpp"

class InvertedIndex {
  public:
    // @param dir - Directory containing inverted index related files, such as
    //      inverted lists, postings files, and lexicon files.
    InvertedIndex(std::string dir);

    // Create inverted index using files in the directory.
    void index();

    // Load inverted index files that need to be in memory, such as the
    // lexicon, from the directory.
    void load();

    // Open an inverted list
    // @param term - Term to open its inverted list
    // @return list ID number, unique for each opened inverted list
    list_p open(std::string term);

    // Close an inverted list
    // @param lp - list ID number, unique for each opened inverted list
    void close(list_p lp);

    // Find the next document ID greater or equal to docID in an open inverted
    // list. If docID is less than the current docID for the list, simply
    // returns the current docID.
    // @param lp - list ID number, unique for each opened inverted list
    // @param docID - document ID number
    // @return document ID greater than or equal to docID
    doc_id next(list_p lp, doc_id docID);

    // Get frequency for the given open inverted list and current document ID
    // @param lp - list ID number, unique for each opened inverted list
    // @return Current frequency
    int getFrequency(list_p lp);

    // Get number of documents in the inverted list
    // @param lp - list ID number, unique for each opened inverted list
    // @return Number of documents in the inverted list
    int getNumDocuments(list_p lp);

  private:
    // Index postings from file containing all merged posting files
    void indexPostings();

    // Reading posting from postings file
    // @return Tuple containing posting as (term, docID, frequency)
    std::tuple<std::string, doc_id, int> readPosting();

    // Push posting to memory. If the posting belongs to a new  term, flush
    // indexed postings to index file first.
    void processPosting(std::tuple<std::string, doc_id, int> posting);

    // Check whether posting is from a new term
    // @param term - Term from the last read posting
    // @return True if it is a new term, false otherwise
    bool isNewTerm(std::string term);

    // Create new inverted list
    // @param term - Term for this list
    void createInvertedList(std::string term);

    // Flush inverted list from memory to inverted index file
    void flushInvertedList();

    // Write number of documents to inverted index file
    // @param numDocs - Number of documents
    void writeNumberOfDocs(uint32_t numDocs);

    // Write document IDs stored in memory to inverted index file
    void writeDocumentIDs();

    // Write frequencies stored in memory to inverted index file
    void writeFrequencies();

    // Write data to inverted index file and update current byte offset
    void write(char* addr, unsigned int size);

    std::shared_ptr<InvertedList> getOpenInvertedList(list_p lp);

    // File stream to read postings
    std::ifstream postingsFileStream;

    // File path to read postings
    std::string postingsPath;

    // File stream to write inverted lists
    std::ofstream indexFileStream;

    // File path to read/write inverted index
    std::string indexPath;

    // File path to read/write lexicon
    std::string lexiconPath;

    // Current inverted index file offset when indexing postings
    uint64_t currentIndexOffset {0};

    // Lexicon used when indexing and searching
    Lexicon lexicon;

    // Current inverted list used when indexing postings
    std::shared_ptr<InvertedList> currentInvertedList;

    // Open inverted lists used when querying
    std::map<list_p, std::shared_ptr<InvertedList>> openLists;
};

#endif // INVERTED_INDEX_HPP
