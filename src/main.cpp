#include <iostream>

#include "document.hpp"
#include "document_table.hpp"
#include "parser.hpp"

int main() {
    LOG_SET_DEBUG();

    std::string path = "../data/CC-MAIN-20190915052433-20190915074433-00000.warc.wet";
    // std::string path = "../data/test.wet";

    LOG_I("Parsing " + path);

    DocumentTable documentTable;
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
        // LOG_D("Words:");
        // for (auto word : document->getWords())
        //     LOG_D(word);

        documentTable.addDocument(document);
    }

    LOG_I("Finished parsing " + path);
    LOG_I("Read " << documentTable.size() << " documents");

    auto documentID = 6;
    LOG_D("Document " << documentID << " URL: " << documentTable.getDocumentURL(documentID));
    LOG_D("Document " << documentID << " Size: " << documentTable.getDocumentSize(documentID));

    return 0;
}