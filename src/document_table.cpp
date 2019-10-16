#include "document_table.hpp"

DocumentTable::DocumentTable(std::string dir) {
    this->documentTableFilePath = dir + "/document-table.txt";
}

int DocumentTable::size() {
    return this->documents.size();
}

std::string DocumentTable::getDocumentURL(doc_id documentID) {
    return std::get<0>(this->documents[documentID]);
}

int DocumentTable::getDocumentSize(doc_id documentID) {
    return std::get<1>(this->documents[documentID]);
}

void DocumentTable::load() {
    std::ifstream fd(this->documentTableFilePath);

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