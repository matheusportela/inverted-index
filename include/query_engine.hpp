/**
    Query engine
*/

#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "document_table.hpp"
#include "inverted_index.hpp"
#include "log.hpp"
#include "types.hpp"

#define NUM_TOP_DOCUMENTS 10

class QueryEngine {
  public:
    QueryEngine(std::string dir);
    void load();
    std::vector<std::tuple<std::string, float, std::vector<int>>> query(std::string query_string);

  private:
    std::vector<std::string> splitQuery(std::string query_string);
    std::vector<std::tuple<doc_id, float, std::vector<int>>> getDocuments(std::vector<std::string> terms);
    float calculateBM25Score(float average_document_size, int document_table_size, int inverted_list_length, int term_frequency, int document_length);
    std::vector<std::tuple<doc_id, float, std::vector<int>>> getTopDocuments(std::vector<std::tuple<doc_id, float, std::vector<int>>> documents);
    std::vector<std::tuple<std::string, float, std::vector<int>>> getTopURLs(std::vector<std::tuple<doc_id, float, std::vector<int>>> top_documents);

    std::string dir;
    std::shared_ptr<DocumentTable> document_table;
    std::shared_ptr<InvertedIndex> inverted_index;
};

#endif // QUERY_ENGINE_HPP
