#include "inverted_list.hpp"

list_p InvertedList::nextAvailableID = 0;

InvertedList::InvertedList(std::string term) : term(term) {
    this->id = this->nextAvailableID;
    this->nextAvailableID++;

    LOG_D("Inverted list ID: " << this->id);
}

// InvertedList::~InvertedList() {
//     if (this->block != NULL)
//         free(this->block);
// }

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

void InvertedList::read(std::ifstream& fd) {
    this->readNumDocs(fd);
    this->readDocumentIDs(fd);
    this->readFrequencies(fd);
    // this->readBlock(fd);
    LOG_D("Number of docs for term '" << term << "': " << this->numDocs);
}

void InvertedList::readNumDocs(std::ifstream& fd) {
    fd.read((char*)&this->numDocs, sizeof(this->numDocs));
}

void InvertedList::readDocumentIDs(std::ifstream& fd) {
    uint8_t* bytes = (uint8_t*)malloc(this->numDocs*sizeof(uint32_t));
    fd.read((char*)bytes, this->numDocs*sizeof(uint32_t));

    this->docIDs.clear();
    uint32_t docID = 0;
    uint32_t compressedDocID = 0;

    for (int i = 0; i < this->numDocs; i++) {
        compressedDocID = bytes[i*sizeof(uint32_t)] | bytes[i*sizeof(uint32_t) + 1] << 8 | bytes[i*sizeof(uint32_t) + 2] << 16 | bytes[i*sizeof(uint32_t) + 3] << 24 ;
        docID = docID + compressedDocID;
        this->docIDs.push_back(docID);
    }

    free(bytes);
}

void InvertedList::readFrequencies(std::ifstream& fd) {
    uint8_t* bytes = (uint8_t*)malloc(this->numDocs*sizeof(uint32_t));
    fd.read((char*)bytes, this->numDocs*sizeof(uint32_t));

    this->frequencies.clear();
    uint32_t freq = 0;

    for (int i = 0; i < this->numDocs; i++) {
        freq = bytes[i*sizeof(uint32_t)] | bytes[i*sizeof(uint32_t) + 1] << 8 | bytes[i*sizeof(uint32_t) + 2] << 16 | bytes[i*sizeof(uint32_t) + 3] << 24 ;
        this->frequencies.push_back(freq);
    }

    free(bytes);
}

doc_id InvertedList::nextGEQ(doc_id docID) {
    if (this->currentIndex == this->numDocs)
        return MAX_DOC_ID;

    do {
        if (this->currentIndex == this->numDocs) {
            // LOG_D("End of inverted list");
            this->currentDocID = MAX_DOC_ID;
            break;
        }

        this->currentDocID = this->docIDs[this->currentIndex];
        this->currentFrequency = this->frequencies[this->currentIndex];
        this->currentIndex++;
        // LOG_D("Current index: " << this->currentIndex);
    } while (this->currentDocID < docID);

    // LOG_D("Returned doc ID: " << this->currentDocID);
    return this->currentDocID;
}
