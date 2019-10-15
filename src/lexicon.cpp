#include "lexicon.hpp"

term_id Lexicon::nextAvailableID = 0;

int Lexicon::size() {
    return this->nextAvailableID;
}

bool Lexicon::contains(std::string term) {
    auto it = this->stringToIDMap.find(term);
    return (it != this->stringToIDMap.end());
}

term_id Lexicon::addTerm(std::string term) {
    term_id termID = nextAvailableID;
    nextAvailableID++;

    this->stringToIDMap[term] = termID;

    return termID;
}

term_id Lexicon::addOrGetTerm(std::string term) {
    if (this->contains(term))
        return this->getID(term);
    else
        return this->addTerm(term);
}

term_id Lexicon::getID(std::string term) {
    return this->stringToIDMap[term];
}

std::string Lexicon::getTerm(term_id id) {
    return this->idToStringMap[id];
}

std::tuple<int, int, int> Lexicon::getMetadata(std::string term) {
    return this->stringToMetadataMap[term];
}

void Lexicon::addTermMetadata(std::string term, int invertedListStart, int invertedListEnd, int numDocs) {
    this->stringToMetadataMap[term] = std::make_tuple(invertedListStart, invertedListEnd, numDocs);
}

void Lexicon::writeIntermediate(std::string path) {
    std::ofstream fd(path);

    for (auto [term, termID] : this->stringToIDMap) {
        fd << termID;
        fd << ' ';
        fd << term;
        fd << '\n';
    }

    fd.close();
}

void Lexicon::readIntermediate(std::string path) {
    std::ifstream fd(path);

    term_id termID;
    std::string term;

    while (fd.good()) {
        fd >> termID;
        fd >> term;

        if (fd.eof())
            break;

        this->idToStringMap[termID] = term;
    }

    fd.close();
}

void Lexicon::write(std::string path) {
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

void Lexicon::read(std::string path) {
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
