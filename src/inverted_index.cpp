#include "inverted_index.hpp"

InvertedIndex::InvertedIndex(std::string path) {
    this->postingsPath = path + "/merged-postings.txt";
    this->indexPath = path + "/index.txt";
    this->lexiconPath = path + "/lexicon.txt";
}

void InvertedIndex::index() {
    // Open file containing all postings to index
    this->postingsFileStream.open(this->postingsPath);

    // Open file to write index in binary format
    // Erase index file if it already exists
    this->indexFileStream.open(this->indexPath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    while (this->postingsFileStream.good()) {
        auto posting = this->readPosting();

        // Check whether file stream is EOF *after* reading post because there
        // is a trailing empty line at the end of the file
        if (this->postingsFileStream.eof())
            break;

        this->processPosting(posting);
    }

    this->flushInvertedList();

    this->postingsFileStream.close();
    this->indexFileStream.close();

    // Write lexicon after indexing postings
    this->lexicon.write(lexiconPath);
}

std::tuple<std::string, doc_id, int> InvertedIndex::readPosting() {
    std::string term;
    doc_id docID;
    int count;

    this->postingsFileStream >> term;
    this->postingsFileStream >> docID;
    this->postingsFileStream >> count;

    return std::make_tuple(term, docID, count);
}

void InvertedIndex::processPosting(std::tuple<std::string, doc_id, int> posting) {
    auto [term, docID, count] = posting;

    if (this->isNewTerm(term)) {
        this->flushInvertedList();
        this->createInvertedList(term);
    }

    this->currentDocIDs.push_back(docID);
    this->currentFrequencies.push_back(count);
}

bool InvertedIndex::isNewTerm(std::string term) {
    return term != this->currentTerm;
}

void InvertedIndex::createInvertedList(std::string term) {
    this->currentTerm = term;
    this->currentDocIDs.clear();
    this->currentFrequencies.clear();
}

void InvertedIndex::flushInvertedList() {
    int numDocs = this->currentDocIDs.size();

    // Skip flushing if there are no postings.
    // This may happen in the very first flush, when a new term is found but
    // no posting is in memory yet.
    if (numDocs == 0)
        return;

    // Byte offset where inverted list starts for the current term
    int listStart = this->currentIndexOffset;

    this->writeNumberOfDocs(numDocs);
    this->writeDocumentIDs();
    this->writeFrequencies();

    // Byte offset where inverted list ends for the current term
    int listEnd = this->currentIndexOffset;

    // Add flushed inverted list to lexicon
    this->lexicon.addTermMetadata(this->currentTerm, listStart, listEnd, numDocs);
}

void InvertedIndex::writeNumberOfDocs(uint32_t numDocs) {
    this->write((char*)&numDocs, sizeof(numDocs));
}

void InvertedIndex::writeDocumentIDs() {
    int previousDocID = 0;
    for (auto docID : this->currentDocIDs) {
        // Write docID as differences for later compression
        uint32_t compressedDocID = docID - previousDocID;
        previousDocID = docID;

        this->write((char*)&compressedDocID, sizeof(compressedDocID));
    }
}

void InvertedIndex::writeFrequencies() {
    for (auto frequency : this->currentFrequencies) {
        uint32_t freq = frequency;
        this->write((char*)&freq, sizeof(freq));
    }
}

void InvertedIndex::write(char* addr, unsigned int size) {
    this->indexFileStream.write(addr, size);
    this->currentIndexOffset += size;
}

std::vector<std::pair<doc_id, int>> InvertedIndex::getInvertedList(std::string path, int listStart) {
    std::ifstream fd(path, std::ofstream::in | std::ofstream::binary);

    // Go to inverted list start
    fd.seekg(listStart);

    // Read number of docs
    uint32_t numDocs;
    fd.read((char*)&numDocs, sizeof(numDocs));

    // Read document IDs
    std::vector<doc_id> documents;
    uint32_t docID = 0;
    uint32_t compressedDocID;

    for (int i = 0; i < numDocs; i++) {
        fd.read((char*)&compressedDocID, sizeof(compressedDocID));

        // Uncompress by accumulating all previous document IDs
        docID += compressedDocID;

        documents.push_back(docID);
    }

    // Read frequencies
    std::vector<int> frequencies;
    uint32_t frequency;

    for (int i = 0; i < numDocs; i++) {
        fd.read((char*)&frequency, sizeof(frequency));

        frequencies.push_back(frequency);
    }

    fd.close();

    std::vector<std::pair<doc_id, int>> result;

    for (int i = 0; i < numDocs; i++)
        result.push_back(std::make_pair(documents[i], frequencies[i]));

    return result;
}
