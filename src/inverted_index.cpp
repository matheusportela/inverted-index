#include "inverted_index.hpp"

InvertedIndex::InvertedIndex(std::string dir) {
    this->postingsPath = dir + "/merged-postings.txt";
    this->indexPath = dir + "/index.txt";
    this->lexiconPath = dir + "/lexicon.txt";
}

void InvertedIndex::index() {
    // Open file to write index in binary format
    // Erase index file if it already exists
    this->indexFileStream.open(this->indexPath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    this->indexPostings();

    this->indexFileStream.close();

    // Save lexicon after indexing postings
    this->lexicon.save(lexiconPath);
}

void InvertedIndex::indexPostings() {
    // Open file containing all postings to index
    this->postingsFileStream.open(this->postingsPath);

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
    uint32_t numDocs = this->currentDocIDs.size();

    // Skip flushing if there are no postings.
    // This may happen in the very first flush, when a new term is found but
    // no posting is in memory yet.
    if (numDocs == 0)
        return;

    // Byte offset where inverted list starts for the current term
    uint64_t listStart = this->currentIndexOffset;

    this->writeNumberOfDocs(numDocs);
    this->writeDocumentIDs();
    this->writeFrequencies();

    // Byte offset where inverted list ends for the current term
    uint64_t listEnd = this->currentIndexOffset;

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

void InvertedIndex::load() {
    this->lexicon.load(this->lexiconPath);
}

std::vector<std::pair<doc_id, int>> InvertedIndex::search(std::string term) {
    auto [invertedListStart, invertedListEnd, numDocs] = this->lexicon.getMetadata(term);

    std::vector<std::pair<doc_id, int>> result;

    if (numDocs > 0)
        result = this->fetchInvertedList(invertedListStart);

    return result;
}

std::vector<std::pair<doc_id, int>> InvertedIndex::fetchInvertedList(uint64_t listStart) {
    std::ifstream fd(this->indexPath, std::ofstream::in | std::ofstream::binary);

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

list_p InvertedIndex::open(std::string term) {
    auto [invertedListStart, _invertedListEnd, _numDocs] = this->lexicon.getMetadata(term);
    std::ifstream fd(this->indexPath, std::ofstream::in | std::ofstream::binary);
    fd.seekg(invertedListStart);

    auto inverted_list = std::make_shared<InvertedList>(term);
    inverted_list->read(fd);

    fd.close();

    this->openLists[inverted_list->getID()] = inverted_list;
    return inverted_list->getID();
}

void InvertedIndex::close(list_p lp) {
    this->openLists.erase(lp);
}

doc_id InvertedIndex::next(list_p lp, doc_id docID) {
    auto inverted_list = this->getOpenInvertedList(lp);
    return inverted_list->nextGEQ(docID);
}

int InvertedIndex::getFrequency(list_p lp) {
    auto inverted_list = this->getOpenInvertedList(lp);
    return inverted_list->getCurrentFrequency();
}

int InvertedIndex::getNumDocuments(list_p lp) {
    auto inverted_list = this->getOpenInvertedList(lp);
    return inverted_list->getNumDocuments();
}

std::shared_ptr<InvertedList> InvertedIndex::getOpenInvertedList(list_p lp) {
    auto it = this->openLists.find(lp);
    if (it == this->openLists.end()) {
        LOG_E("List " << lp << "is not open");
        exit(1);
    }
    return it->second;
}
