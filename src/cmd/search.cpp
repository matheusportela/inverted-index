#include <cctype>
#include <iostream>

#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "log.hpp"

int main() {
    LOG_SET_INFO();

    const std::string path = "../tmp";

    LOG_I("Initializing searcher");

    LOG_I("Loading document table");
    DocumentTable document_table(path);
    document_table.load();

    LOG_I("Loading inverted index lexicon");
    InvertedIndex inverted_index(path);
    inverted_index.load();

    LOG_I("Initialized searcher");

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

        auto list = inverted_index.search(term);
        std::cout << "number of documents: " << list.size() << std::endl;

        if (list.size() == 0)
            continue;

        for (auto [docID, frequency] : list)
            std::cout << docID << " " << frequency << " " << document_table.getDocumentURL(docID) << std::endl;

        std::cout << std::endl;
    }

    return 0;
}
