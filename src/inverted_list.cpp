#include "inverted_list.hpp"

list_p InvertedList::nextAvailableID = 0;

InvertedList::InvertedList(std::string term) : term(term) {
    this->id = this->nextAvailableID;
    this->nextAvailableID++;

    LOG_D("Inverted list ID: " << this->id);
}

InvertedList::~InvertedList() {
    if (this->block != NULL)
        free(this->block);
}

list_p InvertedList::getID() {
    return this->id;
}

std::string InvertedList::getTerm() {
    return this->term;
}

uint32_t InvertedList::getNumDocuments() {
    return this->numDocs;
}

doc_id InvertedList::getCurrentDocID() {
    return this->currentDocID;
}

int InvertedList::getCurrentFrequency() {
    return this->currentFrequency;
}

void InvertedList::addPosting(doc_id docID, int frequency) {
    this->docIDs.push_back(docID);
    this->frequencies.push_back(frequency);
    this->numDocs++;
}

void InvertedList::read(std::ifstream& fd) {
    this->readNumDocs(fd);
    this->readBlock(fd);
    LOG_D("Number of docs for term '" << term << "': " << this->numDocs);
}

void InvertedList::readNumDocs(std::ifstream& fd) {
    fd.read((char*)&this->numDocs, sizeof(this->numDocs));
}

void InvertedList::readBlock(std::ifstream& fd) {
    // 4 bytes for docID, 4 bytes for freqs
    int blockSize = 2*this->numDocs*sizeof(uint32_t);
    this->block = (unsigned char*)malloc(blockSize);

    fd.read((char*)this->block, blockSize);
}

doc_id InvertedList::nextGEQ(doc_id docID) {
    if (this->currentIndex == this->numDocs)
        return INVERTED_LIST_END;

    do {
        if (this->currentIndex == this->numDocs) {
            // LOG_D("End of inverted list");
            this->currentDocID = INVERTED_LIST_END;
            break;
        }

        // Uncompress data
        // Document ID
        auto docIDOffset = this->blockOffset;
        uint32_t compressedDocID = this->block[docIDOffset]
                                 | this->block[docIDOffset + 1] << 8
                                 | this->block[docIDOffset + 2] << 16
                                 | this->block[docIDOffset + 3] << 24;
        uint32_t currentDocID = this->currentDocID + compressedDocID;
        // LOG_D("Block - doc ID offset: " << docIDOffset);
        // LOG_D("Block - doc ID: " << currentDocID);

        // Frequency
        auto freqOffset = this->blockOffset + this->numDocs*sizeof(uint32_t);
        uint32_t currentFrequency = this->block[freqOffset]
                                  | this->block[freqOffset + 1] << 8
                                  | this->block[freqOffset + 2] << 16
                                  | this->block[freqOffset + 3] << 24;
        // LOG_D("Block - freq offset: " << freqOffset);
        // LOG_D("Block - frequency: " << currentFrequency);

        // Bookkeeping
        this->currentDocID = currentDocID;
        this->currentFrequency = currentFrequency;
        this->blockOffset += sizeof(uint32_t);
        this->currentIndex++;
        // LOG_D("Current index: " << this->currentIndex);
    } while (this->currentDocID < docID);

    // LOG_D("Returned doc ID: " << this->currentDocID);
    return this->currentDocID;
}

int InvertedList::write(std::ofstream& fd) {
    int bytes = 0;
    bytes += this->writeNumberOfDocs(fd);
    bytes += this->writeDocumentIDs(fd);
    bytes += this->writeFrequencies(fd);
    return bytes;
}

int InvertedList::writeNumberOfDocs(std::ofstream& fd) {
    fd.write((char*)&this->numDocs, sizeof(this->numDocs));
    return sizeof(this->numDocs);
}

int InvertedList::writeDocumentIDs(std::ofstream& fd) {
    int bytes = 0;
    int previousDocID = 0;
    for (auto docID : this->docIDs) {
        // Write docID as differences for later compression
        uint32_t compressedDocID = docID - previousDocID;
        previousDocID = docID;

        fd.write((char*)&compressedDocID, sizeof(compressedDocID));
        bytes += sizeof(compressedDocID);
    }

    return bytes;
}

int InvertedList::writeFrequencies(std::ofstream& fd) {
    int bytes = 0;

    for (auto frequency : this->frequencies) {
        uint32_t freq = frequency;
        fd.write((char*)&freq, sizeof(freq));
        bytes += sizeof(freq);
    }

    return bytes;
}
