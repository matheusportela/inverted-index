#include "inverted_list.hpp"

list_p InvertedList::nextAvailableID = 0;

InvertedList::InvertedList(std::string term) : term(term) {
    this->id = this->nextAvailableID;
    this->nextAvailableID++;

    LOG_D("Inverted list ID: " << this->id);
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
    this->postings.push_back(std::make_pair(docID, frequency));
    this->numDocs++;
}

int InvertedList::write(std::ofstream& fd) {
    int bytes = 0;
    auto blocks = this->splitBlocks();
    bytes += this->writeMetadata(fd, blocks);
    bytes += this->writeBlocks(fd, blocks);
    return bytes;
}

int InvertedList::writeMetadata(std::ofstream& fd, std::vector<block_t> blocks) {
    int bytes = 0;
    uint32_t numDocs = this->postings.size();
    uint32_t numBlocks = blocks.size();

    fd.write((char*)&numDocs, sizeof(numDocs));
    bytes += sizeof(numDocs);

    fd.write((char*)&numBlocks, sizeof(numBlocks));
    bytes += sizeof(numBlocks);

    return bytes;
}

int InvertedList::writeBlocks(std::ofstream& fd, std::vector<block_t> blocks) {
    int bytes = 0;

    for (auto block : blocks) {
        bytes += this->writeBlock(fd, block);
    }

    return bytes;
}

int InvertedList::writeBlock(std::ofstream& fd, block_t block) {
    int bytes = 0;
    int numDocs = block.size();

    std::vector<doc_id> docIDs;
    std::vector<int> frequencies;

    for (auto [docID, frequency] : block) {
        docIDs.push_back(docID);
        frequencies.push_back(frequency);
    }

    std::vector<uint8_t> compressedDocIDs = this->compressDocumentIDs(docIDs);
    std::vector<uint8_t> compressedFrequencies = this->compressFrequencies(frequencies);

    uint32_t lastDocID = block[numDocs - 1].first;
    // uint32_t docIDsSize = docIDs.size()*sizeof(uint32_t);
    // uint32_t freqsSize = frequencies.size()*sizeof(uint32_t);
    uint32_t docIDsSize = compressedDocIDs.size()*sizeof(uint8_t);
    uint32_t freqsSize = compressedFrequencies.size()*sizeof(uint8_t);
    uint32_t blockSize = sizeof(docIDsSize) + docIDsSize
                       + sizeof(freqsSize) + freqsSize;

    fd.write((char*)&lastDocID, sizeof(lastDocID));
    bytes += sizeof(lastDocID);

    fd.write((char*)&blockSize, sizeof(blockSize));
    bytes += sizeof(blockSize);

    fd.write((char*)&docIDsSize, sizeof(docIDsSize));
    bytes += sizeof(docIDsSize);

    // bytes += this->writeDocumentIDs(fd, docIDs);
    bytes += this->writeByteStream(fd, compressedDocIDs);

    fd.write((char*)&freqsSize, sizeof(freqsSize));
    bytes += sizeof(freqsSize);

    // bytes += this->writeFrequencies(fd, frequencies);
    bytes += this->writeByteStream(fd, compressedFrequencies);

    return bytes;
}

std::vector<uint8_t> InvertedList::compressDocumentIDs(std::vector<doc_id> docIDs) {
    std::vector<uint32_t> numbers;

    doc_id previousDocID = 0;
    for (auto docID : docIDs) {
        // Write docID as differences for later compression
        uint32_t diffDocID = docID - previousDocID;
        previousDocID = docID;

        numbers.push_back(diffDocID);
    }

    return Compression::encode(numbers);
}

std::vector<uint8_t> InvertedList::compressFrequencies(std::vector<int> frequencies) {
    std::vector<uint32_t> numbers;

    for (auto frequency : frequencies) {
        uint32_t freq = frequency;
        numbers.push_back(freq);
    }

    return Compression::encode(numbers);
}

std::vector<block_t> InvertedList::splitBlocks() {
    std::vector<block_t> blocks;
    block_t block;

    for (auto posting : this->postings) {
        if (block.size() == BLOCK_SIZE) {
            blocks.push_back(block);
            block.clear();
        }

        block.push_back(posting);
    }

    if (block.size() > 0)
        blocks.push_back(block);

    return blocks;
}

int InvertedList::writeDocumentIDs(std::ofstream& fd, std::vector<doc_id> docIDs) {
    int bytes = 0;
    doc_id previousDocID = 0;
    for (auto docID : docIDs) {
        // Write docID as differences for later compression
        uint32_t compressedDocID = docID - previousDocID;
        previousDocID = docID;

        fd.write((char*)&compressedDocID, sizeof(compressedDocID));
        bytes += sizeof(compressedDocID);
    }

    return bytes;
}

int InvertedList::writeFrequencies(std::ofstream& fd, std::vector<int> frequencies) {
    int bytes = 0;

    for (auto frequency : frequencies) {
        uint32_t freq = frequency;
        fd.write((char*)&freq, sizeof(freq));
        bytes += sizeof(freq);
    }

    return bytes;
}

int InvertedList::writeByteStream(std::ofstream& fd, std::vector<uint8_t> bytestream) {
    int bytes = 0;

    for (auto b : bytestream) {
        fd.write((char*)&b, sizeof(b));
        bytes += sizeof(b);
    }

    return bytes;
}

void InvertedList::read(std::ifstream& fd) {
    this->docIDs.clear();
    this->frequencies.clear();

    this->readMetadata(fd);
    this->readBlocks(fd);
    LOG_D("Number of docs for term '" << term << "': " << this->numDocs);
}

void InvertedList::readMetadata(std::ifstream& fd) {
    fd.read((char*)&this->numDocs, sizeof(this->numDocs));
    fd.read((char*)&this->numBlocks, sizeof(this->numBlocks));
}

void InvertedList::readBlocks(std::ifstream& fd) {
    for (int i = 0; i < this->numBlocks; i++) {
        this->readBlock(fd);
    }
}

void InvertedList::readBlock(std::ifstream& fd) {
    LOG_D("Reading block");

    uint32_t lastDocID;
    fd.read((char*)&lastDocID, sizeof(lastDocID));
    LOG_D("lastDocID: " << lastDocID);

    uint32_t blockSize;
    fd.read((char*)&blockSize, sizeof(blockSize));
    LOG_D("blockSize: " << blockSize);

    uint32_t docIDsSize;
    fd.read((char*)&docIDsSize, sizeof(docIDsSize));
    LOG_D("docIDsSize: " << docIDsSize);

    this->readDocumentIDs(fd, docIDsSize);

    uint32_t freqsSize;
    fd.read((char*)&freqsSize, sizeof(freqsSize));
    LOG_D("freqsSize: " << docIDsSize);

    this->readFrequencies(fd, freqsSize);
}

void InvertedList::readDocumentIDs(std::ifstream& fd, uint32_t numBytes) {
    auto bytestream = this->readByteStream(fd, numBytes);
    auto diffDocIDs = Compression::decode(bytestream);

    uint32_t docID = 0;
    for (auto diffDocID : diffDocIDs) {
        docID = docID + diffDocID;
        this->docIDs.push_back((int)docID);
    }
}

void InvertedList::readFrequencies(std::ifstream& fd, uint32_t numBytes) {
    auto bytestream = this->readByteStream(fd, numBytes);
    auto frequencies = Compression::decode(bytestream);

    for (auto frequency : frequencies) {
        this->frequencies.push_back(frequency);
    }
}

std::vector<uint8_t> InvertedList::readByteStream(std::ifstream& fd, uint32_t numBytes) {
    std::vector<uint8_t> bytestream;

    uint8_t* bytes = (uint8_t*)malloc(numBytes);
    fd.read((char*)bytes, numBytes);

    for (int i = 0; i < numBytes; i++) {
        bytestream.push_back(bytes[i]);
    }

    free(bytes);

    return bytestream;
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
