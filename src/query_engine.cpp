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

std::vector<std::tuple<std::string, float, std::vector<int>>> QueryEngine::query(std::string query_string) {
    auto terms = this->splitQuery(query_string);
    auto result = this->findTopDocuments(terms);
    return result;
}

std::vector<std::string> QueryEngine::splitQuery(std::string query_string) {
    std::vector<std::string> terms;

    std::istringstream ss(query_string);

    while (ss) {
        std::string term;
        ss >> term;

        if (term.size() > 0) {
            LOG_D("Term: " << term);
            terms.push_back(term);
        }
    };

    return terms;
}

std::vector<std::tuple<std::string, float, std::vector<int>>> QueryEngine::findTopDocuments(std::vector<std::string> terms) {
    // Min-heap to store top documents
    std::priority_queue<std::tuple<float, doc_id, std::vector<int>>, std::vector<std::tuple<float, doc_id, std::vector<int>>>, std::greater<std::tuple<float, doc_id, std::vector<int>>>> top_documents;

    // Open one list per term
    std::vector<list_p> lps;
    for (auto term : terms) {
        auto lp = this->inverted_index->open(term);
        lps.push_back(lp);
    }

    doc_id docID = 0;

    // Score data
    auto average_document_size = this->document_table->getAverageDocumentSize();
    auto document_table_size = this->document_table->size();

    while (docID != MAX_DOC_ID) {
        // Get next post from shortest list
        docID = this->inverted_index->next(lps[0], docID);

        // LOG_D("Doc ID: " << docID);

        // Exit when reaching list end
        if (docID == MAX_DOC_ID) {
            // LOG_D("Exiting due to list end");
            break;
        }

        // Find posting with same document ID in other lists
        doc_id candidateDocID = 0;
        for (int i = 1; (i < terms.size() && (candidateDocID = this->inverted_index->next(lps[i], docID)) == docID); i++);

        if (candidateDocID > docID) {
            // LOG_D("Updating to candidateDocID: " << candidateDocID);
            docID = candidateDocID;
        } else {
            // LOG_D("Calculating score");
            float score = 0;
            std::vector<int> term_frequencies;

            for (auto lp : lps) {
                auto term_frequency = this->inverted_index->getFrequency(lp);
                auto document_size = this->document_table->getDocumentSize(docID);
                auto inverted_list_size = this->inverted_index->getNumDocuments(lp);

                auto term_score = this->calculateBM25Score(average_document_size, document_table_size, inverted_list_size, term_frequency, document_size);

                // LOG_D("Term score: " << term_score);

                score += term_score;

                term_frequencies.push_back(term_frequency);
            }

            // LOG_D("Score: " << score);

            top_documents.push(std::make_tuple(score, docID, term_frequencies));
            if (top_documents.size() > NUM_TOP_DOCUMENTS) {
                top_documents.pop();
            }

            // Go to next document
            // docID++;
        }
    }

    // Close all lists
    for (auto lp : lps) {
        this->inverted_index->close(lp);
    }

    // Convert heap to vector
    std::vector<std::tuple<std::string, float, std::vector<int>>> result;

    while (!top_documents.empty()) {
        auto [score, docID, term_frequencies] = top_documents.top();
        auto url = this->document_table->getDocumentURL(docID);
        result.push_back(std::make_tuple(url, score, term_frequencies));
        top_documents.pop();
    }

    // Invert min heap to max results
    std::sort(result.begin(), result.end(), [](auto &a, auto &b) { return std::get<1>(a) > std::get<1>(b); });

    return result;
}

float QueryEngine::calculateBM25Score(float average_document_size, int document_table_size, int inverted_list_size, int term_frequency, int document_size) {
    // Okapi BM25 ranking function
    // Reference: https://en.wikipedia.org/wiki/Okapi_BM25
    const float k = 1.2;
    const float b = 0.75;

    float idf = log((document_table_size - inverted_list_size + 0.5)/(inverted_list_size + 0.5));

    float score = idf*(term_frequency*(k + 1))/(term_frequency + k*(1 - b + b*document_size/average_document_size));

    return score;
}
