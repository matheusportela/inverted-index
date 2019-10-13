#include "inverted_index.hpp"

void InvertedIndex::add(std::shared_ptr<Document> document) {
    for (auto [termID, frequency] : document->getFrequencies()) {
        auto documentID = document->getID();

        if (this->index.count(termID) == 0)
            index[termID] = std::vector<std::pair<doc_id, int>>();

        index[termID].push_back(std::make_pair(documentID, frequency));
    }
}

std::vector<std::pair<doc_id, int>> InvertedIndex::search(term_id termID) {
    return this->index[termID];
}

void InvertedIndex::buildFromIntermediatePostings(std::string inputPath, std::string outputPath, Lexicon& lexicon) {
    this->input.open(inputPath);
    // this->output.open(outputPath, std::ofstream::out | std::ofstream::trunc);
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

std::tuple<term_id, doc_id, int> InvertedIndex::readPosting() {
    term_id termID;
    doc_id docID;
    int count;

    this->input >> termID;
    this->input >> docID;
    this->input >> count;

    return std::make_tuple(termID, docID, count);
}

void InvertedIndex::processPosting(std::tuple<term_id, doc_id, int> posting, Lexicon& lexicon) {
    auto [termID, docID, count] = posting;

    // LOG_D("Posting: " + std::to_string(termID) + " " + std::to_string(docID) + " " + std::to_string(count));

    if (this->isNewTerm(termID)) {
        this->flushInvertedList(lexicon);
        this->createInvertedList(termID);
    }

    this->currentDocIDs.push_back(docID);
    this->currentFrequencies.push_back(count);
}

bool InvertedIndex::isNewTerm(term_id termID) {
    return termID != this->currentTermID;
}

void InvertedIndex::createInvertedList(term_id termID) {
    this->currentTermID = termID;
    this->currentDocIDs.clear();
    this->currentFrequencies.clear();
}

void InvertedIndex::flushInvertedList(Lexicon& lexicon) {
    // this->output << "t: " << this->currentTermID;
    // this->output << "\ts: ";
    // this->output << this->currentDocIDs.size();
    // this->output << "\td: ";

    // for (auto docID : this->currentDocIDs)
    //     this->output << docID << " ";

    // this->output << "\tf: ";

    // for (auto frequency : this->currentFrequencies)
    //     this->output << frequency << " ";

    // this->output << "\n";

    int listStart = this->currentIndexOffset;

    uint32_t termID = this->currentTermID;
    uint32_t numDocs = this->currentDocIDs.size();

    this->output.write((char*)&termID, sizeof(termID));
    this->currentIndexOffset += sizeof(termID);

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

    std::string term = lexicon.getTerm(termID);
    lexicon.addTermMetadata(term, termID, listStart, listEnd, numDocs);
}

std::vector<doc_id> InvertedIndex::getInvertedList(std::string path, int listStart) {
    std::ifstream fd(path, std::ofstream::in | std::ofstream::binary);

    uint32_t termID;
    uint32_t numDocs;

    fd.seekg(listStart);

    fd.read((char*)&termID, sizeof(termID));
    fd.read((char*)&numDocs, sizeof(numDocs));

    std::vector<doc_id> documents;

    uint32_t docID = 0;
    uint32_t compressedDocID;

    for (int i = 0; i < numDocs; i++) {
        fd.read((char*)&compressedDocID, sizeof(compressedDocID));
        docID += compressedDocID;

        documents.push_back(docID);
    }

    fd.close();

    return documents;
}