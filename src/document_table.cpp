#include "document_table.hpp"

int DocumentTable::size() {
    return this->table.size();
}

void DocumentTable::addDocument(std::shared_ptr<Document> document) {
    auto tuple = std::make_tuple(document->getURL(), document->getSize());
    this->table[document->getID()] = tuple;
}

std::string DocumentTable::getDocumentURL(doc_id documentID) {
    return std::get<0>(this->table[documentID]);
}

int DocumentTable::getDocumentSize(doc_id documentID) {
    return std::get<1>(this->table[documentID]);
}

void DocumentTable::write(std::string path) {
    std::ofstream fd(path);

    for (auto [documentID, metadata] : this->table) {
        auto [url, size] = metadata;

        fd << documentID;
        fd << ' ';
        fd << url;
        fd << ' ';
        fd << size;
        fd << '\n';
    }

    fd.close();
}