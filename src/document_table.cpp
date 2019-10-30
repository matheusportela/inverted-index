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

float DocumentTable::getAverageDocumentSize() {
    return this->average_document_size;
}

void DocumentTable::load() {
    std::ifstream fd(this->documentTableFilePath);

    std::string url;
    int size;

    this->documents.clear();

    this->average_document_size = 0;

    while (fd.good()) {
        fd >> url;
        fd >> size;

        if (fd.eof())
            break;

        // Add to documents list
        this->documents.push_back(std::make_tuple(url, size));

        // Update cumulative moving average
        // Reference: https://en.wikipedia.org/wiki/Moving_average#Cumulative_moving_average
        this->average_document_size = this->average_document_size + (size - this->average_document_size)/(this->documents.size() + 1);
    }

    fd.close();
}