#include "document.hpp"

doc_id Document::nextAvailableID = 0;

Document::Document(std::string url, std::vector<std::pair<term_id, int>> frequencies) :
    id(Document::nextAvailableID), url(url), frequencies(frequencies) {
    Document::nextAvailableID++;
}

doc_id Document::getID() {
    return this->id;
}

std::string Document::getURL() {
    return this->url;
}

std::vector<std::pair<term_id, int>> Document::getFrequencies() {
    return this->frequencies;
}

int Document::getSize() {
    return this->frequencies.size();
}
