#include <iostream>
#include <memory>

#include "document.hpp"
#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "parser.hpp"
#include "types.hpp"

void readIntermediatePostings(std::string path);

int main() {
    LOG_SET_DEBUG();

    DocumentTable documentTable;
    Lexicon lexicon;
    InvertedIndex inverted_index;

    readIntermediatePostings("merged-postings.txt");
    // documentTable.read("document_table.txt");
    // lexicon.read("lexicon.txt");

    return 0;
}

void readIntermediatePostings(std::string path) {
    std::ifstream fd(path);

    term_id termID;
    doc_id docID;
    int count;

    while (fd.good()) {
        fd >> termID;
        fd >> docID;
        fd >> count;

        if (fd.eof())
            break;

        LOG_D("Posting: " + std::to_string(termID) + " " + std::to_string(docID) + " " + std::to_string(count));
    }

    // for (auto [termID, frequencies] : this->index) {
    //     for (auto [docID, count] : frequencies) {
    //         // fd << term_id;
    //         fd << term;
    //         fd << ' ';
    //         fd << '(';
    //         fd << docID;
    //         fd << ' ';
    //         fd << count;
    //         fd << ')';
    //         fd << ' ';
    //         fd << '\n';
    //     }
    // }

    fd.close();
}
