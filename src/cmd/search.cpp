#include <cctype>
#include <iostream>
#include <tuple>

#include "query_engine.hpp"
#include "lexicon.hpp"
#include "log.hpp"

#include "compression.hpp"

int main() {
    LOG_SET_DEBUG();

    const std::string path = "../tmp";

    LOG_I("Initializing query engine");

    LOG_I("Loading document table");

    QueryEngine query_engine(path);
    query_engine.load();

    LOG_I("Initialized query engine");

    std::string query;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, query);

        // Convert query to lowercase
        for (int i = 0; i < query.size(); i++) {
            if ('A' <= query[i] && query[i] <= 'Z')
                query[i] = (char)(query[i] + 32);
        }

        LOG_D("Querying '" << query << "'");

        auto search_result = query_engine.query(query);
        std::cout << "results: ";

        if (search_result.size() == 0)
            continue;

        for (auto [url, score, term_frequencies] : search_result) {
            std::cout << "score: " << score << " ";

            std::cout << "terms: ";
            for (auto term_frequency : term_frequencies)
                std::cout << term_frequency << " ";

            std::cout << "url: " << url << std::endl;
        }

        std::cout << std::endl;
    }

    return 0;
}
