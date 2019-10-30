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

std::vector<std::pair<std::string, float>> QueryEngine::search(std::string term) {
    auto inverted_list = this->getInvertedList(term);
    auto document_scores = this->calculateInvertedListScore(inverted_list);
    auto top_documents = this->getTopDocuments(document_scores);
    auto result = this->getTopURLs(top_documents);
    return result;
}

std::vector<std::pair<doc_id, int>> QueryEngine::getInvertedList(std::string term) {
    return this->inverted_index->search(term);
}

std::vector<std::pair<doc_id, float>> QueryEngine::calculateInvertedListScore(std::vector<std::pair<doc_id, int>> inverted_list) {
    std::vector<std::pair<doc_id, float>> document_scores;
    auto average_document_size = this->document_table->getAverageDocumentSize();
    auto document_table_size = this->document_table->size();
    auto inverted_list_size = inverted_list.size();

    for (auto [docID, term_frequency] : inverted_list) {
        auto document_size = this->document_table->getDocumentSize(docID);
        auto score = this->calculateBM25Score(average_document_size, document_table_size, inverted_list_size, term_frequency, document_size);
        document_scores.push_back(std::make_pair(docID, score));
    }

    return document_scores;
}

float QueryEngine::calculateBM25Score(float average_document_size, int document_table_size, int inverted_list_size, int term_frequency, int document_size) {
    // Okapi BM25 ranking function
    // Reference: https://en.wikipedia.org/wiki/Okapi_BM25
    const float k = 1.5;
    const float b = 0.75;

    float idf = log((document_table_size - inverted_list_size + 0.5)/(inverted_list_size + 0.5));

    float score = idf*(term_frequency*(k + 1))/(term_frequency + k*(1 - b + b*document_size/average_document_size));

    return score;
}

std::vector<std::pair<doc_id, float>> QueryEngine::getTopDocuments(std::vector<std::pair<doc_id, float>> document_scores) {
    sort(document_scores.begin(), document_scores.end(), [](auto &a, auto &b) { return a.second > b.second; });

    std::vector<std::pair<doc_id, float>> top_documents(document_scores.begin(), document_scores.begin() + NUM_TOP_DOCUMENTS);
    return top_documents;
}

std::vector<std::pair<std::string, float>> QueryEngine::getTopURLs(std::vector<std::pair<doc_id, float>> top_documents) {
    std::vector<std::pair<std::string, float>> result;

    for (auto [docID, score] : top_documents) {
        auto url = this->document_table->getDocumentURL(docID);
        result.push_back(std::make_pair(url, score));
    }

    return result;
}
