#include "lexicon.hpp"

std::tuple<int, int, int> Lexicon::getMetadata(std::string term) {
    return this->stringToMetadataMap[term];
}

void Lexicon::addTermMetadata(std::string term, int invertedListStart, int invertedListEnd, int numDocs) {
    this->stringToMetadataMap[term] = std::make_tuple(invertedListStart, invertedListEnd, numDocs);
}

void Lexicon::save(std::string path) {
    std::ofstream fd(path);

    for (auto [term, metadata] : this->stringToMetadataMap) {
        auto [invertedListStart, invertedListEnd, numDocs] = metadata;

        fd << term;
        fd << ' ';
        fd << invertedListStart;
        fd << ' ';
        fd << invertedListEnd;
        fd << ' ';
        fd << numDocs;
        fd << '\n';
    }

    fd.close();
}

void Lexicon::load(std::string path) {
    std::ifstream fd(path);

    std::string term;
    int invertedListStart;
    int invertedListEnd;
    int numDocs;

    while (fd.good()) {
        fd >> term;
        fd >> invertedListStart;
        fd >> invertedListEnd;
        fd >> numDocs;

        if (fd.eof())
            break;

        this->stringToMetadataMap[term] = std::make_tuple(invertedListStart, invertedListEnd, numDocs);
    }

    LOG_D("Total entries in lexicon: " << this->stringToMetadataMap.size());

    fd.close();
}
