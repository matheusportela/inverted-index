#include <iostream>
#include <memory>

#include "document.hpp"
#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "parser.hpp"
#include "types.hpp"

void indexDocument(std::string path, DocumentTable& documentTable, Lexicon& lexicon, InvertedIndex& inverted_index);
void searchDocumentMetadata(doc_id documentID, DocumentTable& documentTable);
void searchTermByString(std::string term, Lexicon& lexicon, InvertedIndex& inverted_index);
void searchTermByID(term_id termID, Lexicon& lexicon, InvertedIndex& inverted_index);

int main() {
    LOG_SET_DEBUG();

    DocumentTable documentTable;
    Lexicon lexicon;

    std::vector<std::string> paths = {
        "../data/CC-MAIN-20190915052433-20190915074433-00000.warc.wet",
        "../data/CC-MAIN-20190915052433-20190915074433-00001.warc.wet",
    };

    for (int i = 0; i < paths.size(); i++) {
        InvertedIndex inverted_index;
        auto input = paths[i];
        auto output = "index" + std::to_string(i) + ".txt";

        indexDocument(paths[i], documentTable, lexicon, inverted_index);
        inverted_index.write_intermediate_index(output, lexicon);
    }

    LOG_I("Document table size: " << documentTable.size());
    LOG_I("Lexicon size: " << lexicon.size());

    documentTable.write("document_table.txt");
    lexicon.write("lexicon.txt");

    // searchDocumentMetadata(6, documentTable);
    // searchTermByString("Google", lexicon, inverted_index);

    // for (int i = 0; i < lexicon.size(); i++)
    //     searchTermByID(i, lexicon, inverted_index);

    return 0;
}

void indexDocument(std::string path, DocumentTable& documentTable, Lexicon& lexicon, InvertedIndex& inverted_index) {
    LOG_I("Indexing " + path);

    int numParsedDocuments = 0;
    Parser parser(path);

    while (!parser.isEOF()) {
        if (numParsedDocuments == 100)
            break;

        auto [url, frequencies] = parser.parseDocument();

        // Ignore parsed documents without URL
        if (url.empty())
            continue;

        // Convert term frequencies to use term IDs
        std::vector<std::pair<term_id, int>> termIDFrequencies;
        for (auto [termString, count] : frequencies) {
            auto termID = lexicon.addOrGetTerm(termString);
            termIDFrequencies.push_back(std::make_pair(termID, count));
        }

        auto document = std::make_shared<Document>(url, termIDFrequencies);

        LOG_D("ID: " << document->getID() <<
              " Size: " << document->getSize() <<
              " URL: " << document->getURL());

        // Update document table
        documentTable.addDocument(document);

        // Add document to inverted index
        inverted_index.add(document);

        numParsedDocuments++;
    }

    LOG_D("Finished indexing " + path);
    LOG_D("Read " << numParsedDocuments << " documents");
}

void searchDocumentMetadata(doc_id documentID, DocumentTable& documentTable) {
    LOG_D("Document " << documentID << " metadata");
    LOG_D("URL: " << documentTable.getDocumentURL(documentID));
    LOG_D("Size: " << documentTable.getDocumentSize(documentID));
}

void searchTermByString(std::string term, Lexicon& lexicon, InvertedIndex& inverted_index) {
    LOG_I("Searching term '" + term + "'");

    if (lexicon.contains(term)) {
        auto termID = lexicon.getID(term);
        LOG_I("Term ID: " << termID);
        for (auto [documentID, frequency] : inverted_index.search(termID)) {
            LOG_D("Document " << documentID << ": " << frequency);
        }
    } else {
        LOG_I("Term '" + term + "' not indexed");
    }
}

void searchTermByID(term_id termID, Lexicon& lexicon, InvertedIndex& inverted_index) {
    std::string term = lexicon.getTerm(termID);
    searchTermByString(term, lexicon, inverted_index);
}