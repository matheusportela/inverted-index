/**
    Lexicon data structure.

    A lexicon is a data structure that, given a term, is able to efficiently
    determine the start and end of the inverted list in the inverted index file
    corresponding to this term as well as the number of documents containing the term. In simple terms, it is a mapping term -> (listStart, listEnd, numDocs).

    A lexicon entry contains:
        - term: Indexed term as string
        - listStart: Byte offset from the start of the inverted index file where
            the inverted list for the term starts
        - listEnd: Byte offset from the start of the inverted index file where
            the inverted list for the term ends
            as uint32
            one stored as uint32
        - numDocs: Number of documents containing the term

    A lexicon is stored in disk as term, listStart, listEnd, and numDocs order,
    one entry per line. There is no particular ordering between entry lines.

    Example:
        brooklyn 103656 103676 2
        fruit 153204 153224 2
        brazil 103088 103116 3
        lady 212080 212100 2
*/

#ifndef LEXICON_HPP
#define LEXICON_HPP

#include <fstream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "log.hpp"

class Lexicon {
  public:
    // @return Number of terms in lexicon
    int size();

    // Get term metadata from lexicon (listStart, listEnd, numDocs)
    // @param term - Indexed term as string
    // @return Tuple containing (listStart, listEnd, numDocs)
    std::tuple<uint64_t, uint64_t, uint32_t> getMetadata(std::string term);

    // Add term metadata to lexicon
    // @param term - Indexed term as string
    // @param listStart - Byte offset where inverted list starts for the term
    // @param listEnd - Byte offset where inverted list ends for the term
    // @param numDocs - Number of documents containing the term
    void addTermMetadata(std::string term, uint64_t listStart, uint64_t listEnd, uint32_t numDocs);

    // Save lexicon to file
    // @param path - File path to write lexicon
    void save(std::string path);

    // Load lexicon from file
    // @param path - File path to read lexicon
    void load(std::string path);

  private:
    std::unordered_map<std::string, std::tuple<uint64_t, uint64_t, uint32_t>> map;
};

#endif // LEXICON_HPP
