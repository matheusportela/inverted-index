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

void DocumentTable::read(std::string path) {
    std::ifstream fd(path);
    doc_id documentID;
    std::string url;
    int size;

    while (fd.good()) {
        fd >> documentID;
        fd >> url;
        fd >> size;

        if (fd.eof())
            break;

        this->table[documentID] = std::make_tuple(url, size);
    }

    fd.close();
}