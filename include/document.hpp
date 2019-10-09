// Document data structure.
//
// A document represents a crawled web page, containing its URL and parsed
// words. Each document has a unique monotonically-increasing ID.
//
// Document is not thread-safe.

#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <string>
#include <vector>

class Document {
  public:
    Document(std::string url, std::vector<std::string> words);

    int getID();
    std::string getURL();
    std::vector<std::string> getWords();
    int getSize();

    static int nextAvailableID;

  private:
    const int id;
    const std::string url;
    const std::vector<std::string> words;
};

#endif // DOCUMENT_HPP
