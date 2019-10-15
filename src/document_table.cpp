#include "document_table.hpp"

int DocumentTable::size() {
    return this->documents.size();
}

void DocumentTable::addDocument(std::shared_ptr<Document> document) {
    auto tuple = std::make_tuple(document->getURL(), document->getSize());
    this->documents.push_back(tuple);
}

std::string DocumentTable::getDocumentURL(doc_id documentID) {
    return std::get<0>(this->documents[documentID]);
}

int DocumentTable::getDocumentSize(doc_id documentID) {
    return std::get<1>(this->documents[documentID]);
}

void DocumentTable::write(std::string path) {
    std::ofstream fd(path);

    for (auto [url, size] : this->documents) {
        fd << url;
        fd << ' ';
        fd << size;
        fd << '\n';
    }

    fd.close();
}

void DocumentTable::read(std::string path) {
    std::ifstream fd(path);

    std::string url;
    int size;

    this->documents.clear();
    while (fd.good()) {
        fd >> url;
        fd >> size;

        if (fd.eof())
            break;

        this->documents.push_back(std::make_tuple(url, size));
    }

    fd.close();
}