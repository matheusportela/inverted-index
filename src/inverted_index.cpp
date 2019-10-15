#include "inverted_index.hpp"

void InvertedIndex::buildFromIntermediatePostings(std::string inputPath, std::string outputPath, Lexicon& lexicon) {
    this->input.open(inputPath);
    this->output.open(outputPath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    while (this->input.good()) {
        auto posting = this->readPosting();

        // Check whether file stream is EOF _after_ reading post because there
        // is a trailing empty line at the end of the file
        if (this->input.eof())
            break;

        this->processPosting(posting, lexicon);
    }

    this->flushInvertedList(lexicon);

    this->input.close();
    this->output.close();
}

std::tuple<std::string, doc_id, int> InvertedIndex::readPosting() {
    std::string term;
    doc_id docID;
    int count;

    this->input >> term;
    this->input >> docID;
    this->input >> count;

    return std::make_tuple(term, docID, count);
}

void InvertedIndex::processPosting(std::tuple<std::string, doc_id, int> posting, Lexicon& lexicon) {
    auto [term, docID, count] = posting;

    if (this->isFirstTerm) {
        this->createInvertedList(term);
        this->isFirstTerm = false;
    } else if (this->isNewTerm(term)) {
        this->flushInvertedList(lexicon);
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

void InvertedIndex::flushInvertedList(Lexicon& lexicon) {
    int listStart = this->currentIndexOffset;

    uint32_t numDocs = this->currentDocIDs.size();
    this->output.write((char*)&numDocs, sizeof(numDocs));
    this->currentIndexOffset += sizeof(numDocs);

    int previousDocID = 0;
    for (auto docID : this->currentDocIDs) {
        uint32_t compressedDocID = docID - previousDocID;
        previousDocID = docID;

        this->output.write((char*)&compressedDocID, sizeof(compressedDocID));
        this->currentIndexOffset += sizeof(compressedDocID);
    }

    for (auto frequency : this->currentFrequencies) {
        uint32_t freq = frequency;
        this->output.write((char*)&freq, sizeof(freq));
        this->currentIndexOffset += sizeof(freq);
    }

    int listEnd = this->currentIndexOffset;

    std::string term = this->currentTerm;
    lexicon.addTermMetadata(term, listStart, listEnd, numDocs);
}

std::vector<std::pair<doc_id, int>> InvertedIndex::getInvertedList(std::string path, int listStart) {
    std::ifstream fd(path, std::ofstream::in | std::ofstream::binary);

    fd.seekg(listStart);

    uint32_t numDocs;
    fd.read((char*)&numDocs, sizeof(numDocs));

    std::vector<doc_id> documents;
    uint32_t docID = 0;
    uint32_t compressedDocID;

    for (int i = 0; i < numDocs; i++) {
        fd.read((char*)&compressedDocID, sizeof(compressedDocID));
        docID += compressedDocID;

        documents.push_back(docID);
    }

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