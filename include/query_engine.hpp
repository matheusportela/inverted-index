/**
    Query engine
*/

#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "document_table.hpp"
#include "inverted_index.hpp"
#include "log.hpp"

#define NUM_TOP_DOCUMENTS 10

class QueryEngine {
  public:
    QueryEngine(std::string dir);
    void load();
    std::vector<std::pair<std::string, float>> search(std::string term);

  private:
    std::vector<std::pair<doc_id, int>> getInvertedList(std::string term);
    std::vector<std::pair<doc_id, float>> calculateInvertedListScore(std::vector<std::pair<doc_id, int>> inverted_list);
    float calculateDocumentScore(int term_frequency);
    std::vector<std::pair<doc_id, float>> getTopDocuments(std::vector<std::pair<doc_id, float>> document_scores);
    std::vector<std::pair<std::string, float>> getTopURLs(std::vector<std::pair<doc_id, float>> top_documents);

    std::string dir;
    std::shared_ptr<DocumentTable> document_table;
    std::shared_ptr<InvertedIndex> inverted_index;
};

#endif // QUERY_ENGINE_HPP
