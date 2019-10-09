#include "inverted_index.hpp"

void InvertedIndex::add(std::shared_ptr<Document> document) {
    for (auto [term, frequency] : document->getFrequencies()) {
        auto documentID = document->getID();

        if (this->index.count(term) == 0)
            index[term] = std::vector<std::pair<int, int>>();

        index[term].push_back(std::make_pair(documentID, frequency));
    }
}

std::vector<std::pair<int, int>> InvertedIndex::search(std::string term) {
    return this->index[term];
}