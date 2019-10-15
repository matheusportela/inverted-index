#include <cctype>
#include <iostream>

#include "document.hpp"
#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "log.hpp"

int main() {
    // LOG_SET_DEBUG();
    LOG_SET_INFO();

    const std::string dir = "../tmp";
    const std::string documentTableInputPath = dir + "/document-table.txt";
    const std::string lexiconInputPath = dir + "/lexicon.txt";
    const std::string indexInputPath = dir + "/index.txt";

    DocumentTable document_table;
    Lexicon lexicon;
    InvertedIndex inverted_index;

    LOG_I("Initializing searcher");

    LOG_I("Reading document table");
    document_table.read(documentTableInputPath);

    LOG_I("Reading lexicon");
    lexicon.read(lexiconInputPath);

    LOG_I("Initialized searcher");

    std::string term;

    while (true) {
        std::cout << "> ";
        std::cin >> term;

        for (int i = 0; i < term.size(); i++)
            term[i] = (char)std::tolower(term[i]);

        LOG_D("Searching for term '" << term << "'");

        // auto [termID, invertedListStart, invertedListEnd, numDocs] = lexicon.getMetadata(term);
        auto [invertedListStart, invertedListEnd, numDocs] = lexicon.getMetadata(term);

        // LOG_D("termID: " << termID);
        LOG_D("invertedListStart: " << invertedListStart);
        LOG_D("invertedListEnd: " << invertedListEnd);
        LOG_D("numDocs: " << numDocs);

        std::cout << "number of documents: " << numDocs << std::endl;

        if (numDocs == 0)
            continue;

        auto list = inverted_index.getInvertedList(indexInputPath, invertedListStart);

        for (auto [docID, frequency] : list)
            std::cout << docID << " " << frequency << " " << document_table.getDocumentURL(docID) << std::endl;

        std::cout << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
