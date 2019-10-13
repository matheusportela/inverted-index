#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "log.hpp"

int main() {
    LOG_SET_DEBUG();

    const std::string dir = "../tmp";
    const std::string intermediateLexiconPath = dir + "/lexicon-intermediate.txt";
    const std::string postingsPath = dir + "/merged-postings.txt";
    const std::string indexOutputPath = dir + "/index.txt";
    const std::string lexiconOutputPath = dir + "/lexicon.txt";

    LOG_I("Reading intermediate lexicon");

    Lexicon lexicon;
    lexicon.readIntermediate(intermediateLexiconPath);

    LOG_I("Creating inverted index");

    InvertedIndex inverted_index;
    inverted_index.buildFromIntermediatePostings(postingsPath, indexOutputPath, lexicon);

    LOG_I("Created inverted index");

    LOG_I("Writing lexicon to " << lexiconOutputPath);
    lexicon.write(lexiconOutputPath);

    return 0;
}
