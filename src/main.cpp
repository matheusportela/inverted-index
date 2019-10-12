#include <iostream>

#include "document.hpp"
#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "parser.hpp"

int main() {
    LOG_SET_DEBUG();

    std::string path = "../data/CC-MAIN-20190915052433-20190915074433-00000.warc.wet";
    // std::string path = "../data/test.wet";

    LOG_I("Parsing " + path);

    InvertedIndex index;
    DocumentTable documentTable;
    Lexicon lexicon;

    Parser parser(path);

    while (!parser.isEOF()) {
        if (documentTable.size() == 10)
            break;

        auto document = parser.parseDocument();
        if (!document)
            continue;

        LOG_D("ID: " << document->getID());
        LOG_D("URL: " + document->getURL());
        LOG_D("Size: " << document->getSize());
        // LOG_D("Terms:");
        // for (auto term : document->getTerms())
        //     LOG_D(term);

        documentTable.addDocument(document);
        index.add(document);
    }

    LOG_I("Finished parsing " + path);
    LOG_I("Read " << documentTable.size() << " documents");

    auto documentID = 6;
    LOG_D("Document " << documentID << " URL: " << documentTable.getDocumentURL(documentID));
    LOG_D("Document " << documentID << " Size: " << documentTable.getDocumentSize(documentID));

    std::string term = "10";
    LOG_D("Searching term '" + term + "' in index");
    for (auto [documentID, frequency] : index.search(term)) {
        LOG_D("Document " << documentID << ": " << frequency);
    }

    return 0;
}