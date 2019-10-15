#include <filesystem>
#include <iostream>
#include <memory>

#include "document.hpp"
#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "parser.hpp"
#include "types.hpp"

std::vector<std::string> getFilePaths(std::string directoryPath);
std::string getPostingPath(std::string postingsPath, int postingNumber);
void createIntermediatePostings(std::string inputPath, std::string outputPath, std::ofstream& documentTableFileStream, Lexicon& lexicon);
void writeIntermediatePostings(std::ofstream& fd, std::shared_ptr<Document> document);
void writeDocumentTableEntry(std::ofstream& fd, std::shared_ptr<Document> document);

int main() {
    LOG_SET_INFO();

    const std::string inputDir = "../data/common-crawl";
    const std::string outputDir = "../tmp";
    const std::string documentTablePath = outputDir + "/document-table.txt";
    const std::string lexiconPath = outputDir + "/lexicon-intermediate.txt";

    LOG_I("Creating intermediate postings");

    DocumentTable documentTable;
    Lexicon lexicon;

    std::vector<std::string> paths = getFilePaths(inputDir);

    // Erase document table file if existing
    std::ofstream documentTableFileStream(documentTablePath, std::ofstream::out | std::ofstream::trunc);

    for (int i = 0; i < paths.size(); i++) {
        auto input = paths[i];
        auto output = getPostingPath(outputDir, i);

        createIntermediatePostings(input, output, documentTableFileStream, lexicon);
    }

    documentTableFileStream.close();

    LOG_I("Created intermediate postings");

    return 0;
}

std::vector<std::string> getFilePaths(std::string directoryPath) {
    std::vector<std::string> paths;

    for (auto entry : std::filesystem::directory_iterator(directoryPath))
        paths.push_back(entry.path());

    return paths;
}

std::string getPostingPath(std::string postingsPath, int postingNumber) {
    return postingsPath + "/postings-" + std::to_string(postingNumber) + ".txt";
}

void createIntermediatePostings(std::string inputPath, std::string outputPath, std::ofstream& documentTableFileStream, Lexicon& lexicon) {
    LOG_I("Creating intermediate postings for " + inputPath);

    // Erase output file if existing
    std::ofstream fd(outputPath, std::ofstream::out | std::ofstream::trunc);

    int numParsedDocuments = 0;
    Parser parser(inputPath);

    while (!parser.isEOF()) {
        // if (numParsedDocuments == 100)
        //     break;

        auto [url, frequencies] = parser.parseDocument();

        // Ignore parsed documents without URL
        if (url.empty())
            continue;

        // auto document = std::make_shared<Document>(url, termIDFrequencies);
        auto document = std::make_shared<Document>(url, frequencies);

        LOG_D("ID: " << document->getID() <<
              " Size: " << document->getSize() <<
              " URL: " << document->getURL());

        // Save postings to file
        writeIntermediatePostings(fd, document);

        // Save document metadata to document table file
        writeDocumentTableEntry(documentTableFileStream, document);

        numParsedDocuments++;
    }

    fd.close();

    LOG_D("Processed " + inputPath);
    LOG_D("Read " << numParsedDocuments << " documents");
}

void writeIntermediatePostings(std::ofstream& fd, std::shared_ptr<Document> document) {
    auto docID = document->getID();
    auto frequencies = document->getFrequencies();

    for (auto [term, count] : frequencies) {
        fd << term;
        fd << ' ';
        fd <<  docID;
        fd << ' ';
        fd <<  count;
        fd << '\n';
    }
}

void writeDocumentTableEntry(std::ofstream& fd, std::shared_ptr<Document> document) {
    auto url = document->getURL();
    auto size = document->getSize();

    fd << url;
    fd << ' ';
    fd << size;
    fd << '\n';
}
