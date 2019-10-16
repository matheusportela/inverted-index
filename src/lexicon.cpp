#include "lexicon.hpp"

std::tuple<uint64_t, uint64_t, uint32_t> Lexicon::getMetadata(std::string term) {
    return this->map[term];
}

void Lexicon::addTermMetadata(std::string term, uint64_t listStart, uint64_t listEnd, uint32_t numDocs) {
    this->map[term] = std::make_tuple(listStart, listEnd, numDocs);
}

void Lexicon::save(std::string path) {
    std::ofstream fd(path);

    for (auto [term, metadata] : this->map) {
        auto [listStart, listEnd, numDocs] = metadata;

        fd << term;
        fd << ' ';
        fd << listStart;
        fd << ' ';
        fd << listEnd;
        fd << ' ';
        fd << numDocs;
        fd << '\n';
    }

    fd.close();
}

void Lexicon::load(std::string path) {
    std::ifstream fd(path);

    std::string term;
    uint64_t listStart;
    uint64_t listEnd;
    uint32_t numDocs;

    while (fd.good()) {
        fd >> term;
        fd >> listStart;
        fd >> listEnd;
        fd >> numDocs;

        if (fd.eof())
            break;

        this->map[term] = std::make_tuple(listStart, listEnd, numDocs);
    }

    LOG_D("Total entries in lexicon: " << this->map.size());

    fd.close();
}
