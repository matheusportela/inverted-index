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

    LOG_D("Reading document table");
    document_table.read(documentTableInputPath);

    LOG_D("Reading lexicon");
    lexicon.read(lexiconInputPath);

    LOG_I("Searching");

    std::string term;

    while (true) {
        std::cout << "> ";
        std::cin >> term;

        LOG_D("Searching for term '" << term << "'");

        auto [termID, invertedListStart, invertedListEnd, numDocs] = lexicon.getMetadata(term);

        LOG_D("termID: " << termID);
        LOG_D("invertedListStart: " << invertedListStart);
        LOG_D("invertedListEnd: " << invertedListEnd);
        LOG_D("numDocs: " << numDocs);

        if (numDocs == 0) {
            std::cout << "not found" << std::endl;
            continue;
        }

        auto documents = inverted_index.getInvertedList(indexInputPath, invertedListStart);

        for (auto docID : documents)
            std::cout << docID << " " << document_table.getDocumentURL(docID) << std::endl;
    }

    return 0;
}
