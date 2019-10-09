#include "document_table.hpp"

int DocumentTable::size() {
    return this->table.size();
}

void DocumentTable::addDocument(std::shared_ptr<Document> document) {
    auto tuple = std::make_tuple(document->getURL(), document->getSize());
    this->table[document->getID()] = tuple;
}

std::string DocumentTable::getDocumentURL(int documentID) {
    return std::get<0>(this->table[documentID]);
}

int DocumentTable::getDocumentSize(int documentID) {
    return std::get<1>(this->table[documentID]);
}