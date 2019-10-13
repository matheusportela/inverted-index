#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "log.hpp"

int main() {
    LOG_SET_DEBUG();

    const std::string dir = "../tmp";
    const std::string lexiconPath = dir + "/lexicon-intermediate.txt";
    const std::string postingsPath = dir + "/merged-postings.txt";
    const std::string outputPath = dir + "/index.txt";

    LOG_I("Reading intermediate lexicon");

    Lexicon lexicon;
    lexicon.readIntermediate(lexiconPath);

    LOG_I("Creating inverted index");

    InvertedIndex inverted_index;
    inverted_index.buildFromIntermediatePostings(postingsPath, outputPath);

    LOG_I("Created inverted index");

    return 0;
}
