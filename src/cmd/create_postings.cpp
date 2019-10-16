#include <filesystem>

#include "document_parser.hpp"

std::vector<std::string> getFilePaths(std::string directoryPath);
std::string getPostingPath(std::string postingsPath, int postingNumber);

int main() {
    LOG_SET_INFO();

    const std::string inputDir = "../data/common-crawl";
    const std::string outputDir = "../tmp";
    const std::string documentTablePath = outputDir + "/document-table.txt";
    const std::string lexiconPath = outputDir + "/lexicon-intermediate.txt";

    LOG_I("Creating postings and document table");

    DocumentParser documentParser(documentTablePath);

    std::vector<std::string> paths = getFilePaths(inputDir);

    for (int i = 0; i < paths.size(); i++) {
        auto input = paths[i];
        auto output = getPostingPath(outputDir, i);
        documentParser.parseWETFile(input, output);
    }

    LOG_I("Created postings and document table");

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
