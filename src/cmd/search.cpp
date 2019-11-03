#include <cctype>
#include <iostream>
#include <tuple>

#include "query_engine.hpp"
#include "lexicon.hpp"
#include "log.hpp"

#include "compression.hpp"

int main() {
    LOG_SET_DEBUG();

    uint64_t start = 2778428808;
    uint64_t end = 2778435800;
    auto bytes = end - start;
    std::ifstream fd;
    fd.open("../tmp/index.txt", std::ofstream::in | std::ofstream::binary);
    fd.seekg(start);

    uint8_t buffer[bytes];
    fd.read((char*)buffer, bytes);

    fd.close();

    for (int i = 0; i < bytes; i++) {
        auto byte = buffer[i];
        std::cout << (int)byte << " ";
    }
    std::cout << std::endl;

    return 0;


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
        std::cout << "number of documents: " << search_result.size() << std::endl;

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
