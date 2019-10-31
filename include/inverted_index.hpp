/**
    Inverted index data structure.

    An inverted index is a data structure that, given a term, is able to
    efficiently determine all indexed documents containing the aforementioned
    term. It is a collection of inverted lists, one for each indexed term.

    Usually, the inverted index is too large to fit main memory, so we store the
    inverted lists on a file. In order to be able to efficiently fetch inverted
    lists from disk, the inverted index uses a lexicon (which fits in main
    memory) - a data structure that can efficiently determine the inverted list  address in the index file for any given a term. Having this information, the
    inverted index seeks this position and read binary data sequentially.

    An inverted list contains:
        - numDocs: Number of documents containing the term stored as uint32
        - docIDs: Vector of document IDs containing the term, each one stored
            as uint32
        - freqs: Vector of term frequency for the respective document ID, each
            one stored as uint32

    Document IDs are stored in delta format. A docID that will be stored in
    position N in the vector will be stored as docID[N] - docID[N-1]. Likewise,
    when retrieving the docID from the list, the docID in position N is the sum
    of all docIDs before it: docID = docID[0] + docID[1] + ... + doc[N-1].

    For instance, given the following postings:
        icp 0 3
        icp 2 1
        icp 3 1
        icp 6 2
        icp 7 5

    The inverted list will look like the following:
        5 0 2 1 3 1 3 1 1 2 5

    where:
        5: number of documents
        0 2 1 3 1: docIDs stored as deltas
        3 1 1 2 5: frequencies stored as is

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

#include "lexicon.hpp"
#include "log.hpp"
#include "types.hpp"

#define INVERTED_LIST_END 1000000000

struct list_descriptor {
    uint64_t indexOffset;

    unsigned int numDocs;
    int currentIndex;
    doc_id currentDocID;
    int currentFrequency;

    unsigned char* block;
    unsigned int blockOffset;
};

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

    // Search for term in lexicon and retrieve inverted list from index file.
    // @param term - Term to be searched
    // @result Vector containing pairs of (docID, frequency)
    std::vector<std::pair<doc_id, int>> search(std::string term);

    list_p open(std::string term);
    void close(list_p lp);
    doc_id next(list_p lp, doc_id docID);
    bool end(list_p lp);
    void readBlock(list_p lp);
    int getFrequency(list_p lp);
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

    void assertListIsOpen(list_p lp);

    // Fetch inverted list from the inverted index file
    // @param listStart - Address of the inverted list start in the inverted
    //      index file.
    std::vector<std::pair<doc_id, int>> fetchInvertedList(uint64_t listStart);

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

    // Current term when indexing postings
    std::string currentTerm;

    // Current document IDs when indexing postings
    std::vector<doc_id> currentDocIDs;

    // Current frequencies when indexing postings
    std::vector<int> currentFrequencies;

    // Current inverted index file offset when indexing postings
    uint64_t currentIndexOffset {0};

    // Lexicon used when indexing and searching
    Lexicon lexicon;

    list_p next_list_pointer {0};
    std::map<list_p, struct list_descriptor> list_pointer_table;
};

#endif // INVERTED_INDEX_HPP
