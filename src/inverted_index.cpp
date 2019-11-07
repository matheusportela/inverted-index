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

    // Flush last processed inverted list
    this->flushInvertedList();

    this->postingsFileStream.close();
}

std::tuple<std::string, doc_id, int> InvertedIndex::readPosting() {
    std::string term;
    doc_id docID;
    int frequency;

    this->postingsFileStream >> term;
    this->postingsFileStream >> docID;
    this->postingsFileStream >> frequency;

    return std::make_tuple(term, docID, frequency);
}

void InvertedIndex::processPosting(std::tuple<std::string, doc_id, int> posting) {
    auto [term, docID, frequency] = posting;

    if (this->currentInvertedList == nullptr) {
        this->createInvertedList(term);
    } else if (this->currentInvertedList->getTerm() != term) {
        this->flushInvertedList();
        this->createInvertedList(term);
    }

    this->currentInvertedList->addPosting(docID, frequency);
}

bool InvertedIndex::isNewTerm(std::string term) {
    return this->currentInvertedList == nullptr || this->currentInvertedList->getTerm() != term;
}

void InvertedIndex::createInvertedList(std::string term) {
    this->currentInvertedList = std::make_shared<InvertedList>(term);
}

void InvertedIndex::flushInvertedList() {
    uint32_t numDocs = this->currentInvertedList->getNumDocuments();

    // Byte offset where inverted list starts for the current term
    uint64_t listStart = this->currentIndexOffset;

    // Write inverted list to file and update offset
    this->currentIndexOffset += this->currentInvertedList->write(this->indexFileStream);

    // Byte offset where inverted list ends for the current term
    uint64_t listEnd = this->currentIndexOffset;

    // Add flushed inverted list to lexicon
    this->lexicon.addTermMetadata(this->currentInvertedList->getTerm(), listStart, listEnd, numDocs);
}

void InvertedIndex::load() {
    this->lexicon.load(this->lexiconPath);
}

list_p InvertedIndex::open(std::string term) {
    auto [invertedListStart, invertedListEnd, numDocs] = this->lexicon.getMetadata(term);
    auto inverted_list = std::make_shared<InvertedList>(term);

    // Only read from file when term has documents as returned by the lexicon
    LOG_I("Number of docs containing term " << term << ": " << numDocs);
    if (numDocs > 0) {
        inverted_list->open(this->indexPath, invertedListStart);
    } else {
        inverted_list->close();
    }

    this->openLists[inverted_list->getID()] = inverted_list;
    return inverted_list->getID();
}

void InvertedIndex::close(list_p lp) {
    this->openLists[lp]->close();
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
