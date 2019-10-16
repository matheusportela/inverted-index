#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "log.hpp"

int main() {
    LOG_SET_INFO();

    const std::string path = "../tmp";

    LOG_I("Creating inverted index");

    InvertedIndex inverted_index(path);
    inverted_index.index();

    LOG_I("Created inverted index");

    return 0;
}
