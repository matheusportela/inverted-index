/**
    Query engine
*/

#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <queue>
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
    enum QueryMode {
        CONJUNCTIVE,
        DISJUNCTIVE
    };

    QueryEngine(std::string dir);
    void load();
    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::vector<int>, std::string>> query(std::string query_string);

  private:
    std::pair<QueryMode, std::string> processQueryMode(std::string query_string);
    std::vector<std::string> splitQuery(std::string query_string);
    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::vector<int>, std::string>> runConjunctiveQuery(std::vector<std::string> terms);
    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::vector<int>, std::string>> runDisjunctiveQuery(std::vector<std::string> terms);
    float calculateBM25Score(float average_num_terms, int document_table_size, int inverted_list_length, int term_frequency, int document_length);
    std::string generateSnippet(doc_id docID, std::vector<std::string> terms);
    std::string generateSnippetForTerm(doc_id docID, std::string term);

    std::string dir;
    std::shared_ptr<DocumentTable> document_table;
    std::shared_ptr<InvertedIndex> inverted_index;
};

#endif // QUERY_ENGINE_HPP
