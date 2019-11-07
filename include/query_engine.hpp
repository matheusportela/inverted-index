/**
    Query engine.

    Having generated the inverted index, lexicon and document table, the query
    engine provides methods to perform queries on the indexed data.

    This is done by first loading both the document table and lexicon from disk
    to memory. Then, given a term, it executes either conjunctive or disjunctive
    query to find the top 10 documents, using the BM25 ranking score. For
    efficiency purposes, the query engine never loads the entire inverted lists
    associated to the terms to RAM. Instead, it reads one block at a time and
    try and skip as many blocks as possible.

    In general, query execution runs the following steps:

    - Get the inverted list start position for the terms using the lexicon
    - Seek to the inverted index file position
    - Find top document IDs with highest scores for each term according to the query semantics
    - Fetch document IDs metadata from document table
    - Generate snippets
    - Return results

    The results is a vector of at most 10 tuples containing the following
    information:

    - URL: Document URL
    - score: Ranking score
    - doc_size: Number of terms in this document
    - term_frequencies: Inverted list size for each term in the query
    - inverted_list_sizes: Frequency of each query term in this document
    - snippet: Text snippet showing the context where the terms appear
*/

#ifndef QUERY_ENGINE_HPP
#define QUERY_ENGINE_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "document_table.hpp"
#include "inverted_index.hpp"
#include "log.hpp"
#include "types.hpp"

// Number of documents to return in a query
#define NUM_TOP_DOCUMENTS 10

class QueryEngine {
  public:
    enum QueryMode {
        CONJUNCTIVE,
        DISJUNCTIVE
    };

    // @param dir - Directory containing inverted index related files, such as
    //      inverted lists, postings files, and lexicon files.
    QueryEngine(std::string dir);

    // Load lexicon and document table to memory
    void load();

    // Return the top documents for the given query and related metadata
    // @param query_string - String containing terms to be searched
    // @return Vector containing [(URL, score, doc_size, term_frequencies, inverted_list_sizes, snippet)]
    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::vector<int>, std::string>> query(std::string query_string);

  private:
    // Define which query more to use in the given query. If the query starts
    // with "%and ", use conjunctive mode. If it starts with "%or ", use
    // disjunctive mode. Defaults to conjunctive mode.
    // @param query_string - String containing terms to be searched
    // @return Tuple of query mode and the remaining query string
    std::pair<QueryMode, std::string> processQueryMode(std::string query_string);

    // Split query string into vector of terms
    // @param query_string - String containing terms to be searched
    // @return Vector of terms to be searched
    std::vector<std::string> splitQuery(std::string query_string);

    // Perform conjunctive query
    // @param terms - Vector of terms to be searched
    // @return Top 10 documents containing all terms in the query
    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::vector<int>, std::string>> runConjunctiveQuery(std::vector<std::string> terms);

    // Perform disjunctive query
    // @param terms - Vector of terms to be searched
    // @return Top 10 documents containing at least one term in the query
    std::vector<std::tuple<std::string, float, int, std::vector<int>, std::vector<int>, std::string>> runDisjunctiveQuery(std::vector<std::string> terms);

    // Calculate document ranking score using Okapi BM25
    // @param average_num_terms - Average number of terms per document in the //    dataset
    // @param document_table_size - Number of indexed documents
    // @param inverted_list_length - Number of postings indexed in the current
    //    inverted list
    // @param term_frequency - Number of times the term appeared in the document
    // @param document_length - Number of terms in the document
    // @return Document score
    float calculateBM25Score(float average_num_terms, int document_table_size, int inverted_list_length, int term_frequency, int document_length);

    // Generate snippet showing the first context where the terms appeared in
    // the document
    // @param docID - Document containing terms
    // @param terms - List of terms to generate a snippet
    // @return Snippet showing the context where terms appear in the document
    std::string generateSnippet(doc_id docID, std::vector<std::string> terms);

    // Generate snippet showing the first context where the term appeared in
    // the document
    // @param docID - Document containing terms
    // @param term - Terms to generate a snippet
    // @return Snippet of at most 30 characters showing the context where terms
    //    appear in the document when they do. Otherwise, empty string
    std::string generateSnippetForTerm(doc_id docID, std::string term);

    // Directory containing document table, lexicon and inverted index
    std::string dir;

    // Document table data structure
    std::shared_ptr<DocumentTable> document_table;

    // Inverted index data structure
    std::shared_ptr<InvertedIndex> inverted_index;
};

#endif // QUERY_ENGINE_HPP
