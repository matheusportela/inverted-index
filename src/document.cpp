#include "document.hpp"

int Document::nextAvailableID = 0;

Document::Document(std::string url, std::vector<std::pair<std::string, int>> frequencies) :
    id(Document::nextAvailableID), url(url), frequencies(frequencies) {
    Document::nextAvailableID++;
}

int Document::getID() {
    return this->id;
}

std::string Document::getURL() {
    return this->url;
}

std::vector<std::pair<std::string, int>> Document::getFrequencies() {
    return this->frequencies;
}

int Document::getSize() {
    return this->frequencies.size();
}
