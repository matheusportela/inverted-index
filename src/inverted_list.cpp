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
    // this->docIDs.clear();
    // this->frequencies.clear();

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

    // LOG_D("Read inverted list metadata for term " << this->term);
}

void InvertedList::readBlockMetadata(std::ifstream& fd) {
    // LOG_D("Reading block metadata for term " << this->term);

    this->blockLastDocID = this->readUInt32(fd);
    // LOG_D("blockLastDocID: " << this->blockLastDocID);

    this->blockSize = this->readUInt32(fd);
    // LOG_D("blockSize: " << this->blockSize);

    this->docIDs.clear();
    this->frequencies.clear();

    // this->currentIndex = 0;
    this->numBlocksRead++;
    // LOG_D("Read block metadata for term " << this->term);
}

void InvertedList::readBlock(std::ifstream& fd) {
    // LOG_D("Reading block for term " << this->term);

    uint32_t docIDsSize = this->readUInt32(fd);
    // LOG_D("docIDsSize: " << docIDsSize);

    this->readDocumentIDs(fd, docIDsSize);

    uint32_t freqsSize = this->readUInt32(fd);
    // LOG_D("freqsSize: " << freqsSize);

    this->readFrequencies(fd, freqsSize);
    // LOG_D("Read block for term " << this->term);
}

void InvertedList::readDocumentIDs(std::ifstream& fd, uint32_t numBytes) {
    // LOG_D("Reading doc IDs");
    auto bytestream = this->readByteStream(fd, numBytes);
    auto diffDocIDs = Compression::decode(bytestream);

    // LOG_D("Compressed doc IDs");
    for (auto b : bytestream) {
        // LOG_D(std::bitset<8>(b));
    }

    // LOG_D("Decompressed doc IDs");
    uint32_t docID = 0;
    for (auto diffDocID : diffDocIDs) {
        docID = docID + diffDocID;
        // this->docIDs.push_back((int)docID);
        this->blockDocIDs.push(docID);

        // LOG_D(docID);
    }
    // LOG_D("Read doc IDs: " << this->blockDocIDs.size());
}

void InvertedList::readFrequencies(std::ifstream& fd, uint32_t numBytes) {
    // LOG_D("Reading frequencies");
    auto bytestream = this->readByteStream(fd, numBytes);
    auto frequencies = Compression::decode(bytestream);

    // LOG_D("Compressed frequencies");
    // for (auto b : bytestream) {
    //     LOG_D(std::bitset<8>(b));
    // }

    for (auto frequency : frequencies) {
        // this->frequencies.push_back(frequency);
        this->blockFrequencies.push(frequency);

        // LOG_D(frequency);
    }
    // LOG_D("Read frequencies: " << this->blockFrequencies.size());
}

uint32_t InvertedList::readUInt32(std::ifstream& fd) {
    uint32_t result;

    uint8_t buffer[4];
    fd.read((char*)buffer, sizeof(uint8_t)*4);

    // Little endian
    result = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;

    // fd.read((char*)&result, sizeof(result));

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
    // LOG_D("Opening inverted list term " << this->term);
    this->indexFileStream.open(path, std::ofstream::in | std::ofstream::binary);
    this->indexFileStream.seekg(offset);
    this->read(this->indexFileStream);
    // LOG_D("Opened inverted list term " << this->term);
}

void InvertedList::close() {
    // LOG_D("Closing inverted list term " << this->term);
    if (this->indexFileStream.is_open()) {
        this->indexFileStream.close();
    }
    // LOG_D("Closed inverted list term " << this->term);
}

doc_id InvertedList::nextGEQ(doc_id docID) {
    // LOG_D("Getting next doc from inverted list term " << this->term);

    // LOG_D("Current docID: " << docID);
    // LOG_D("Block last docID: " << this->blockLastDocID);

    if (this->hasReadAllDocuments()) {
        // LOG_D("Reached end of inverted list");
        this->currentDocID = MAX_DOC_ID;
        // this->currentIndex = this->numDocs;
        return MAX_DOC_ID;
    }

    do {
        // Read next block when necessary
        if (this->hasReadAllDocumentsInBlock()) {
            // LOG_D("Reading next block");

            while(true) {
                // Stop skipping blocks when all blocks has been read
                if (this->hasReadAllBlocks()) {
                    // LOG_D("Reached end of all blocks");
                    this->currentDocID = MAX_DOC_ID;
                    // this->currentIndex = this->numDocs;
                    return MAX_DOC_ID;
                }

                // Read next block metadata
                this->readBlockMetadata(this->indexFileStream);

                // Read next block when docID is inside and exit loop
                // or skip next block
                if (this->shouldReadBlock(docID)) {
                    // LOG_D("Reading block data");
                    this->readBlock(this->indexFileStream);
                    break;
                } else {
                    // LOG_D("Skipping block");
                    this->skipBlock(this->indexFileStream);
                }
            }
        }

        this->currentDocID = this->blockDocIDs.front();
        this->currentFrequency = this->blockFrequencies.front();

        this->blockDocIDs.pop();
        this->blockFrequencies.pop();

        // this->currentDocID = this->docIDs[this->currentIndex];
        // this->currentFrequency = this->frequencies[this->currentIndex];
        // this->currentIndex++;
        // LOG_D("Current index: " << this->currentIndex);
    } while (this->currentDocID < docID);

    // LOG_D("Returned doc ID: " << this->currentDocID);
    return this->currentDocID;
}

void InvertedList::skipBlock(std::ifstream& fd) {
    fd.ignore(this->blockSize);
}

bool InvertedList::shouldReadBlock(doc_id docID) {
    return docID <= this->blockLastDocID;
}

bool InvertedList::hasReadAllDocuments() {
    // return this->currentIndex == this->numDocs;
    return this->hasReadAllBlocks() && this->hasReadAllDocumentsInBlock();
}

bool InvertedList::hasReadAllDocumentsInBlock() {
    // return this->currentIndex == this->docIDs.size();
    return this->blockDocIDs.empty();
}

bool InvertedList::hasReadAllBlocks() {
    return this->numBlocksRead == this->numBlocks;
}