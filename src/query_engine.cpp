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

std::vector<std::tuple<std::string, float, int, std::vector<int>, std::string>> QueryEngine::query(std::string query_string) {
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

std::vector<std::tuple<std::string, float, int, std::vector<int>, std::string>> QueryEngine::findTopDocuments(std::vector<std::string> terms) {
    LOG_D("Finding top documents");

    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::string>> result;
    if (terms.size() == 0) {
        return result;
    }

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
    auto average_num_terms = this->document_table->getAverageNumberOfTerms();
    auto document_table_size = this->document_table->size();

    while (docID != MAX_DOC_ID) {
        // LOG_D("docID: " << docID);

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

                auto term_score = this->calculateBM25Score(average_num_terms, document_table_size, inverted_list_size, term_frequency, document_size);

                // LOG_D("Term score: " << term_score);

                score += term_score;

                term_frequencies.push_back(term_frequency);
            }

            // LOG_D("Score: " << score);

            // Add to heap document to heap if
            // 1 - there aren't 10 docs yet
            // 2 - score is greater than minimum score
            if (top_documents.size() < NUM_TOP_DOCUMENTS || score > std::get<0>(top_documents.top())) {
                // LOG_D("Adding document to top documents");
                top_documents.push(std::make_tuple(score, docID, term_frequencies));

                // Remove document with smallest score when heap is full
                if (top_documents.size() > NUM_TOP_DOCUMENTS) {
                    top_documents.pop();
                }
            } else {
                // LOG_D("Discarding document due to low score");
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
    while (!top_documents.empty()) {
        auto [score, docID, term_frequencies] = top_documents.top();

        auto snippet = this->generateSnippet(docID, terms);

        auto url = this->document_table->getDocumentURL(docID);
        auto document_size = this->document_table->getDocumentSize(docID);
        result.push_back(std::make_tuple(url, score, document_size, term_frequencies, snippet));
        top_documents.pop();
    }

    // Reverse min heap to max results
    std::reverse(result.begin(), result.end());

    return result;
}

float QueryEngine::calculateBM25Score(float average_num_terms, int document_table_size, int inverted_list_size, int term_frequency, int document_size) {
    // Okapi BM25 ranking function
    // Reference: https://en.wikipedia.org/wiki/Okapi_BM25
    const float k = 1.2;
    const float b = 0.75;

    float idf = log((document_table_size - inverted_list_size + 0.5)/(inverted_list_size + 0.5));

    float score = idf*(term_frequency*(k + 1))/(term_frequency + k*(1 - b + b*document_size/average_num_terms));

    return score;
}

std::string QueryEngine::generateSnippet(doc_id docID, std::vector<std::string> terms) {
    std::string snippet;

    for (auto term : terms) {
        std::string term_snippet;
        std::vector<std::string> term_variations = { term + " " , " " + term };
        bool is_term_in_snippet = false;

        for (auto term_variation : term_variations) {
            if (snippet.find(term_variation) != std::string::npos) {
                is_term_in_snippet = true;
                break;
            }
        }

        if (is_term_in_snippet)
            continue;

        for (auto term_variation : term_variations) {
            term_snippet = this->generateSnippetForTerm(docID, term_variation);

            if (!term_snippet.empty()) {
                break;
            }
        }

        if (snippet.empty()) {
            snippet = term_snippet;
        } else {
            snippet += " ... " + term_snippet;
        }
    }

    return snippet;
}

std::string QueryEngine::generateSnippetForTerm(doc_id docID, std::string term) {
    auto text = this->document_table->getDocumentText(docID);

    auto term_position = text.find(term);

    if (term_position == std::string::npos)
        return "";

    auto snippet_length = 30;

    if (term_position + snippet_length > text.size())
        snippet_length = text.size() - term_position;

    auto snippet = text.substr(term_position, snippet_length);

    return snippet;
}
