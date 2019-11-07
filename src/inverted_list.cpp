#include "inverted_list.hpp"
#include <bitset>

list_p InvertedList::nextAvailableID = 0;

InvertedList::InvertedList(std::string term) : term(term) {
    this->id = this->nextAvailableID;
    this->nextAvailableID++;

    LOG_D("Inverted list ID: " << this->id);
}

InvertedList::~InvertedList() {
    this->close();
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
    uint32_t docIDsSize = compressedDocIDs.size()*sizeof(uint8_t);
    uint32_t freqsSize = compressedFrequencies.size()*sizeof(uint8_t);
    uint32_t blockSize = sizeof(docIDsSize) + docIDsSize
                       + sizeof(freqsSize) + freqsSize;

    // Block metadata
    fd.write((char*)&lastDocID, sizeof(lastDocID));
    bytes += sizeof(lastDocID);

    fd.write((char*)&blockSize, sizeof(blockSize));
    bytes += sizeof(blockSize);

    // Block data
    fd.write((char*)&docIDsSize, sizeof(docIDsSize));
    bytes += sizeof(docIDsSize);

    bytes += this->writeByteStream(fd, compressedDocIDs);

    fd.write((char*)&freqsSize, sizeof(freqsSize));
    bytes += sizeof(freqsSize);

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

int InvertedList::writeByteStream(std::ofstream& fd, std::vector<uint8_t> bytestream) {
    int bytes = 0;

    for (uint8_t b : bytestream) {
        fd.write((char*)&b, sizeof(b));
        bytes += sizeof(b);
    }

    return bytes;
}

void InvertedList::read(std::ifstream& fd) {
    this->readMetadata(fd);
    this->readBlockMetadata(fd);
    this->readBlock(fd);
}

void InvertedList::readMetadata(std::ifstream& fd) {
    LOG_D("Reading inverted list metadata for term " << this->term);

    this->numDocs = this->readUInt32(fd);
    LOG_D("Num docs: " << this->numDocs);

    this->numBlocks = this->readUInt32(fd);
    LOG_D("Num blocks: " << this->numBlocks);
}

void InvertedList::readBlockMetadata(std::ifstream& fd) {
    this->blockLastDocID = this->readUInt32(fd);
    this->blockSize = this->readUInt32(fd);
    this->docIDs.clear();
    this->frequencies.clear();
    this->numBlocksRead++;
}

void InvertedList::readBlock(std::ifstream& fd) {
    uint32_t docIDsSize = this->readUInt32(fd);
    this->readDocumentIDs(fd, docIDsSize);

    uint32_t freqsSize = this->readUInt32(fd);
    this->readFrequencies(fd, freqsSize);

    // Initialize current doc ID
    this->currentDocID = this->blockDocIDs.front();
    this->blockDocIDs.pop();

    // Initialize current freq
    this->currentFrequency = this->blockFrequencies.front();
    this->blockFrequencies.pop();
}

void InvertedList::readDocumentIDs(std::ifstream& fd, uint32_t numBytes) {
    auto bytestream = this->readByteStream(fd, numBytes);
    auto diffDocIDs = Compression::decode(bytestream);

    uint32_t docID = 0;
    for (auto diffDocID : diffDocIDs) {
        docID = docID + diffDocID;
        this->blockDocIDs.push(docID);
    }
}

void InvertedList::readFrequencies(std::ifstream& fd, uint32_t numBytes) {
    auto bytestream = this->readByteStream(fd, numBytes);
    auto frequencies = Compression::decode(bytestream);

    for (auto frequency : frequencies) {
        this->blockFrequencies.push(frequency);
    }
}

uint32_t InvertedList::readUInt32(std::ifstream& fd) {
    uint32_t result;

    uint8_t buffer[4];
    fd.read((char*)buffer, sizeof(uint8_t)*4);

    // Read in little-endian order
    result = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;

    return result;
}

std::vector<uint8_t> InvertedList::readByteStream(std::ifstream& fd, uint32_t numBytes) {
    std::vector<uint8_t> bytestream;

    uint8_t bytes[numBytes];
    fd.read((char*)bytes, numBytes);

    for (int i = 0; i < numBytes; i++) {
        bytestream.push_back(bytes[i]);
    }

    return bytestream;
}

void InvertedList::open(std::string path, uint64_t offset) {
    this->indexFileStream.open(path, std::ofstream::in | std::ofstream::binary);
    this->indexFileStream.seekg(offset);
    this->read(this->indexFileStream);
}

void InvertedList::close() {
    if (this->indexFileStream.is_open()) {
        this->indexFileStream.close();
    }

    this->currentDocID = MAX_DOC_ID;
}

doc_id InvertedList::nextGEQ(doc_id docID) {
    if (this->currentDocID >= docID) {
        return this->currentDocID;
    }

    if (this->hasReadAllDocuments()) {
        this->currentDocID = MAX_DOC_ID;
        return MAX_DOC_ID;
    }

    do {
        if (this->hasReadAllDocumentsInBlock()) {
            while(true) {
                // Stop skipping blocks when all blocks has been read
                if (this->hasReadAllBlocks()) {
                    this->currentDocID = MAX_DOC_ID;
                    return MAX_DOC_ID;
                }

                // Read next block metadata
                this->readBlockMetadata(this->indexFileStream);

                // Read next block when docID is inside and exit loop
                // or skip next block
                if (this->shouldReadBlock(docID)) {
                    this->readBlock(this->indexFileStream);
                    break;
                } else {
                    this->skipBlock(this->indexFileStream);
                }
            }
        }

        this->currentDocID = this->blockDocIDs.front();
        this->currentFrequency = this->blockFrequencies.front();

        this->blockDocIDs.pop();
        this->blockFrequencies.pop();
    } while (this->currentDocID < docID);

    return this->currentDocID;
}

void InvertedList::skipBlock(std::ifstream& fd) {
    fd.ignore(this->blockSize);
}

bool InvertedList::shouldReadBlock(doc_id docID) {
    return docID <= this->blockLastDocID;
}

bool InvertedList::hasReadAllDocuments() {
    return this->hasReadAllBlocks() && this->hasReadAllDocumentsInBlock();
}

bool InvertedList::hasReadAllDocumentsInBlock() {
    return this->blockDocIDs.empty();
}

bool InvertedList::hasReadAllBlocks() {
    return this->numBlocksRead == this->numBlocks;
}