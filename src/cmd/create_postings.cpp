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
void createIntermediatePostings(std::string inputPath, std::string outputPath, DocumentTable& documentTable, Lexicon& lexicon);
void writeIntermediatePostings(std::string path, std::shared_ptr<Document> document);

int main() {
    // LOG_SET_DEBUG();
    LOG_SET_INFO();

    const std::string inputDir = "../data/common-crawl";
    const std::string outputDir = "../tmp";
    const std::string documentTablePath = outputDir + "/document-table.txt";
    const std::string lexiconPath = outputDir + "/lexicon-intermediate.txt";

    LOG_I("Creating intermediate structures");

    DocumentTable documentTable;
    Lexicon lexicon;

    std::vector<std::string> paths = getFilePaths(inputDir);

    for (int i = 0; i < paths.size(); i++) {
        auto input = paths[i];
        auto output = getPostingPath(outputDir, i);

        // Erase output file if existing
        std::ofstream fd(output, std::ofstream::out | std::ofstream::trunc);
        fd.close();

        createIntermediatePostings(input, output, documentTable, lexicon);
    }

    LOG_I("Creating document table of size " << documentTable.size() << " at " << documentTablePath);
    LOG_I("Creating intermediate lexicon of size " << lexicon.size() << " at " << lexiconPath);

    documentTable.write(documentTablePath);
    lexicon.writeIntermediate(lexiconPath);

    LOG_I("Created intermediate structures");

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

void createIntermediatePostings(std::string inputPath, std::string outputPath, DocumentTable& documentTable, Lexicon& lexicon) {
    LOG_I("Creating intermediate postings for " + inputPath);

    int numParsedDocuments = 0;
    Parser parser(inputPath);

    while (!parser.isEOF()) {
        // if (numParsedDocuments == 100)
        //     break;

        auto [url, frequencies] = parser.parseDocument();

        // Ignore parsed documents without URL
        if (url.empty())
            continue;

        // Convert term frequencies to use term IDs
        std::vector<std::pair<term_id, int>> termIDFrequencies;
        for (auto [termString, count] : frequencies) {
            auto termID = lexicon.addOrGetTerm(termString);
            termIDFrequencies.push_back(std::make_pair(termID, count));
        }

        auto document = std::make_shared<Document>(url, termIDFrequencies);

        LOG_D("ID: " << document->getID() <<
              " Size: " << document->getSize() <<
              " URL: " << document->getURL());

        // Update document table
        documentTable.addDocument(document);

        // Save postings to file
        writeIntermediatePostings(outputPath, document);

        numParsedDocuments++;
    }

    LOG_D("Processed " + inputPath);
    LOG_D("Read " << numParsedDocuments << " documents");
}

void writeIntermediatePostings(std::string path, std::shared_ptr<Document> document) {
    auto docID = document->getID();
    auto frequencies = document->getFrequencies();

    std::ofstream fd(path, std::ofstream::out | std::ofstream::app);

    for (auto [termID, count] : frequencies) {
        fd << std::setfill('0') << std::setw(9) << termID;
        fd << ' ';
        fd <<  std::setfill('0') << std::setw(9) << docID;
        fd << ' ';
        fd <<  std::setfill('0') << std::setw(9) << count;
        fd << '\n';
    }

    // std::ofstream fd(path, std::ofstream::out | std::ofstream::app | std::ofstream::binary);

    // for (auto [termID, count] : frequencies) {
    //     uint32_t termIDValue = termID;
    //     uint32_t docIDValue = docID;
    //     uint32_t countValue = count;

    //     fd.write((char*)&termIDValue, sizeof(termIDValue));
    //     fd.write((char*)&docIDValue, sizeof(docIDValue));
    //     fd.write((char*)&countValue, sizeof(countValue));
    // }

    fd.close();
}
