/**
    Query engine
*/

#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "document_table.hpp"
#include "inverted_index.hpp"
#include "log.hpp"

class QueryEngine {
  public:
    QueryEngine(std::string dir);
    void load();
    std::vector<std::pair<std::string, int>> search(std::string term);

  private:
    std::string dir;
    std::shared_ptr<DocumentTable> document_table;
    std::shared_ptr<InvertedIndex> inverted_index;
};

#endif // QUERY_ENGINE_HPP
