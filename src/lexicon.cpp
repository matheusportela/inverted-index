#include "lexicon.hpp"

term_id Lexicon::nextAvailableID = 0;

term_id Lexicon::addTerm(std::string term) {
    term_id termID = nextAvailableID;
    nextAvailableID++;

    this->stringToIDMap[term] = termID;
    this->idToStringMap[termID] = term;

    return termID;
}

term_id Lexicon::getID(std::string term) {
    return this->stringToIDMap[term];
}

std::string Lexicon::getTerm(term_id id) {
    return this->idToStringMap[id];
}