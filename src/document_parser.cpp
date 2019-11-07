#include "document_parser.hpp"

DocumentParser::DocumentParser(std::string inputDir, std::string outputDir) : inputDir(inputDir), outputDir(outputDir) {
    const std::string documentTablePath = outputDir + "/document-table.txt";

    // Erase document table file if it already exists
    this->documentTableFileStream.open(documentTablePath, std::ofstream::out | std::ofstream::trunc);
}

DocumentParser::~DocumentParser() {
    this->documentTableFileStream.close();
}

void DocumentParser::parse() {
    std::vector<std::string> paths = this->getFilePaths(this->inputDir);

    for (int i = 0; i < paths.size(); i++) {
        auto input = paths[i];
        auto output = this->generatePostingPath(this->outputDir, i);
        this->parseWETFile(input, output);
    }
}

std::vector<std::string> DocumentParser::getFilePaths(std::string directoryPath) {
    std::vector<std::string> paths;

    for (auto entry : std::filesystem::directory_iterator(directoryPath))
        paths.push_back(entry.path());

    return paths;
}

std::string DocumentParser::generatePostingPath(std::string outputDir, int postingNumber) {
    return outputDir + "/postings-" + std::to_string(postingNumber) + ".txt";
}

void DocumentParser::parseWETFile(std::string wetFilePath, std::string postingFilePath) {
    LOG_I("Parsing documents from " + wetFilePath);

    // Erase postings file if it already exists
    std::ofstream postingsFileStream(postingFilePath, std::ofstream::out | std::ofstream::trunc);

    int numParsedDocuments = 0;

    WETParser parser(wetFilePath);

    while (!parser.isEOF()) {
        auto [url, frequencies, document_begin, document_length] = parser.parseDocument();

        // Ignore parsed documents without URL
        if (url.empty())
            continue;

        auto docID = this->generateDocumentID();

        // Calculate document size based on the number of parsed terms
        auto num_terms = frequencies.size();

        // Save parsed postings to postings file
        this->appendPostingEntries(postingsFileStream, docID, frequencies);

        // Save document table entry to document table file
        this->appendDocumentTableEntry(this->documentTableFileStream, url, num_terms, document_begin, document_length, wetFilePath);

        numParsedDocuments++;
    }

    postingsFileStream.close();

    LOG_D("Parsed " + wetFilePath);
    LOG_D("Processed " << numParsedDocuments << " documents");
}

void DocumentParser::appendPostingEntries(std::ofstream& fd, doc_id docID, std::vector<std::pair<std::string, int>> frequencies) {
    for (auto [term, frequency] : frequencies)
        this->appendPostingEntry(fd, docID, term, frequency);
}

void DocumentParser::appendPostingEntry(std::ofstream& fd, doc_id docID, std::string term, int frequency) {
    fd << term;
    fd << ' ';
    fd <<  docID;
    fd << ' ';
    fd <<  frequency;
    fd << '\n';
}

void DocumentParser::appendDocumentTableEntry(std::ofstream& fd, std::string url, int num_terms, uint64_t document_begin, uint64_t document_length, std::string path) {
    fd << url;
    fd << ' ';
    fd << num_terms;
    fd << ' ';
    fd << document_begin;
    fd << ' ';
    fd << document_length;
    fd << ' ';
    fd << path.substr(59, 5); // only write WET file number
    fd << '\n';
}

doc_id DocumentParser::generateDocumentID() {
    doc_id docID = this->nextAvailableDocumentID;
    this->nextAvailableDocumentID++;
    return docID;
}