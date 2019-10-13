#include "inverted_index.hpp"
#include "log.hpp"

int main() {
    LOG_SET_DEBUG();

    LOG_I("Creating inverted index");

    InvertedIndex inverted_index;
    inverted_index.buildFromIntermediatePostings("merged-postings.txt", "index.txt");

    LOG_I("Finished creating inverted index");

    return 0;
}
