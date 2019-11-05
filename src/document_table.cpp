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

uint64_t DocumentTable::getDocumentBegin(doc_id documentID) {
    return std::get<2>(this->documents[documentID]);
}

uint64_t DocumentTable::getDocumentLength(doc_id documentID) {
    return std::get<3>(this->documents[documentID]);
}

std::string DocumentTable::getDocumentPath(doc_id documentID) {
    std::string wet_file_number = std::get<4>(this->documents[documentID]);
    std::string path = "../data/common-crawl/CC-MAIN-20190915052433-20190915074433-" + wet_file_number + ".warc.wet";
    return path;
}

std::string DocumentTable::getDocumentText(doc_id docID) {
    auto document_path = this->getDocumentPath(docID);
    auto document_begin = this->getDocumentBegin(docID);
    auto document_length = this->getDocumentLength(docID);

    char buffer[document_length];

    std::ifstream fd;
    fd.open(document_path);
    fd.seekg(document_begin);
    fd.read(buffer, document_length);
    fd.close();

    char c;
    for (int i = 0; i < document_length; i++) {
        c = buffer[i];

        if (('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
            // Convert to lowercase
            if ('A' <= c && c <= 'Z')
                buffer[i] = (char)(c + 32);
        } else {
            // Remove invalid chars
            buffer[i] = ' ';
        }
    }

    return buffer;
}

float DocumentTable::getAverageNumberOfTerms() {
    return this->average_num_terms;
}

void DocumentTable::load() {
    std::ifstream fd(this->documentTableFilePath);

    std::string url;
    int num_terms;
    uint64_t document_begin;
    uint64_t document_length;
    std::string wet_file_number;

    this->documents.clear();

    this->average_num_terms = 0;

    while (fd.good()) {
        fd >> url;
        fd >> num_terms;
        fd >> document_begin;
        fd >> document_length;
        fd >> wet_file_number;

        if (fd.eof())
            break;

        // Add to documents list
        this->documents.push_back(std::make_tuple(url, num_terms, document_begin, document_length, wet_file_number));

        // Update cumulative moving average
        // Reference: https://en.wikipedia.org/wiki/Moving_average#Cumulative_moving_average
        this->average_num_terms = this->average_num_terms + (num_terms - this->average_num_terms)/(this->documents.size() + 1);
    }

    fd.close();
}