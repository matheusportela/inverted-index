#include <iostream>

#include "parser.hpp"

int main() {
    LOG_SET_DEBUG();

    std::string path = "../data/CC-MAIN-20190915052433-20190915074433-00000.warc.wet";
    // std::string path = "../data/test.wet";

    LOG_I("Parsing " + path);

    Parser parser(path);

    int count = 0;

    while (!parser.isEOF()) {
        auto [url, words] = parser.getDocument();
        count++;
        // LOG_D("URL: " + url);
        // LOG_D("Words:");
        // for (auto word : words)
        //     LOG_D(word);
    }

    LOG_I("Finished parsing " + path);
    LOG_I("Read " + std::to_string(count) + " documents");

    return 0;
}