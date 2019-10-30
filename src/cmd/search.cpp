#include <cctype>
#include <iostream>

#include "query_engine.hpp"
#include "lexicon.hpp"
#include "log.hpp"

int main() {
    LOG_SET_INFO();

    const std::string path = "../tmp";

    LOG_I("Initializing query engine");

    LOG_I("Loading document table");

    QueryEngine query_engine(path);
    query_engine.load();

    LOG_I("Initialized query engine");

    std::string term;

    while (true) {
        std::cout << "> ";
        std::cin >> term;

        // Convert term to lowercase
        for (int i = 0; i < term.size(); i++) {
            if ('A' <= term[i] && term[i] <= 'Z')
                term[i] = (char)(term[i] + 32);
        }

        LOG_D("Searching for term '" << term << "'");

        auto search_result = query_engine.search(term);
        std::cout << "number of documents: " << search_result.size() << std::endl;

        if (search_result.size() == 0)
            continue;

        for (auto [url, frequency] : search_result)
            std::cout << frequency << " " << url << std::endl;

        std::cout << std::endl;
    }

    return 0;
}
