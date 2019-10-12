#include "inverted_index.hpp"

void InvertedIndex::add(std::shared_ptr<Document> document) {
    for (auto [termID, frequency] : document->getFrequencies()) {
        auto documentID = document->getID();

        if (this->index.count(termID) == 0)
            index[termID] = std::vector<std::pair<doc_id, int>>();

        index[termID].push_back(std::make_pair(documentID, frequency));
    }
}

std::vector<std::pair<doc_id, int>> InvertedIndex::search(term_id termID) {
    return this->index[termID];
}

void InvertedIndex::dump(std::string path, Lexicon lexicon) {
    std::ofstream fd(path);

    for (auto [termID, frequencies] : this->index) {
        std::string term = lexicon.getTerm(termID);

        // fd << term_id;
        fd << term;
        fd << ' ';

        for (auto [docID, count] : frequencies) {
            fd << '(';
            fd << docID;
            fd << ' ';
            fd << count;
            fd << ')';
            fd << ' ';
        }

        fd << '\n';
    }

    fd.close();
}
