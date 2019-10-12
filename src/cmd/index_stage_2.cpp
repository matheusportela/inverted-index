#include <iostream>
#include <memory>

#include "document.hpp"
#include "document_table.hpp"
#include "inverted_index.hpp"
#include "lexicon.hpp"
#include "parser.hpp"
#include "types.hpp"

int main() {
    LOG_SET_DEBUG();

    DocumentTable documentTable;
    Lexicon lexicon;
    InvertedIndex inverted_index;

    inverted_index.read_intermediate_postings("merged-index.txt");
    // documentTable.read("document_table.txt");
    // lexicon.read("lexicon.txt");

    return 0;
}
