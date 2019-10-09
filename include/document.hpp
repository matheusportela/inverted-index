// Document data structure.
//
// A document represents a crawled web page, containing its URL and parsed
// terms. Each document has a unique monotonically-increasing ID.
//
// Document is not thread-safe.

#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <map>
#include <utility>
#include <string>
#include <vector>

class Document {
  public:
    Document(std::string url, std::vector<std::pair<std::string, int>> frequencies);

    int getID();
    std::string getURL();
    std::vector<std::pair<std::string, int>> getFrequencies();
    int getSize();

    static int nextAvailableID;

  private:
    const int id;
    const std::string url;
    const std::vector<std::pair<std::string, int>> frequencies;
};

#endif // DOCUMENT_HPP
