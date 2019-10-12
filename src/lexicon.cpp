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
    this->idToStringMap[termID] = term;

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