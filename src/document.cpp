#include "document.hpp"

int Document::nextAvailableID = 0;

Document::Document(std::string url, std::vector<std::string> words) :
    id(Document::nextAvailableID), url(url), words(words) {
    Document::nextAvailableID++;
}

int Document::getID() {
    return this->id;
}

std::string Document::getURL() {
    return this->url;
}

std::vector<std::string> Document::getWords() {
    return this->words;
}

int Document::getSize() {
    return this->words.size();
}
