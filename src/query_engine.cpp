#include "query_engine.hpp"

QueryEngine::QueryEngine(std::string dir) {
    this->dir = dir;
    this->document_table = std::make_shared<DocumentTable>(dir);
    this->inverted_index = std::make_shared<InvertedIndex>(dir);
}

void QueryEngine::load() {
    LOG_I("Loading document table");
    this->document_table->load();
    this->inverted_index->load();
}


std::vector<std::pair<std::string, int>> QueryEngine::search(std::string term) {
    std::vector<std::pair<std::string, int>> result;

    auto inverted_list = this->inverted_index->search(term);

    for (auto [docID, frequency] : inverted_list) {
        auto url = this->document_table->getDocumentURL(docID);
        result.push_back(std::make_pair(url, frequency));
    }

    return result;
}
