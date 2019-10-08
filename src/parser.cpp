#include "parser.hpp"

Parser::Parser(std::string path) : path(path), eof(false) {
    this->infile.open(path);
}

std::pair<std::string, std::vector<std::string>> Parser::getDocument() {
    auto lines = this->getDocumentLines();
    auto headers = this->getDocumentHeaders(lines);
    auto content = this->getDocumentContent(lines);
    auto url = this->getDocumentURL(headers);
    auto words = this->getDocumentWords(content);
    return std::pair(url, words);
}

std::vector<std::string> Parser::getDocumentLines() {
    std::vector<std::string> lines;
    std::string line;

    while (true) {
        line = this->getLine();

        // Stop reading when EOF
        if (this->isEOF())
            break;

        // Stop reading document when next document begins
        if (!lines.empty() && this->isDocumentBegin(line))
            break;

        lines.push_back(line);
    }

    return lines;
}

std::string Parser::getLine() {
    std::stringstream ss;
    std::string line;

    // Read line from input stream char by char
    char c;
    while (this->infile.get(c) && c != '\n')
        ss << c;
    line = ss.str();

    // Check for EOF
    if (this->infile.eof())
        this->setEOF();

    // Sanitize line
    if (line[line.size() - 1] == '\r')
        line.pop_back();

    return line;
}

void Parser::setEOF() {
    this->eof = true;
}

bool Parser::isEOF() {
    return this->eof;
}

bool Parser::isDocumentBegin(std::string line) {
    return line == "WARC/1.0";
}

std::vector<std::string> Parser::getDocumentHeaders(std::vector<std::string> lines) {
    std::vector<std::string> headers;

    for (auto line : lines) {
        headers.push_back(line);

        if (this->isLastHeader(line))
            break;
    }

    return headers;
}

bool Parser::isLastHeader(std::string line) {
    return line.rfind("Content-Length: ", 0) == 0;
}

std::vector<std::string> Parser::getDocumentContent(std::vector<std::string> lines) {
    std::vector<std::string> content;
    int i = 0;

    for (i = 0; i < lines.size(); i++) {
        if (this->isLastHeader(lines[i])) {
            i++;
            break;
        }
    }

    for (; i < lines.size(); i++)
        content.push_back(lines[i]);

    return content;
}

std::string Parser::getDocumentURL(std::vector<std::string> headers) {
    std::string url;

    for (auto header : headers) {
        if (this->isURLHeader(header))
            url = header.substr(std::string("WARC-Target-URI: ").size());
    }

    return url;
}

bool Parser::isURLHeader(std::string line) {
    return line.rfind("WARC-Target-URI: ", 0) == 0;
}

std::vector<std::string> Parser::getDocumentWords(std::vector<std::string> lines) {
    std::vector<std::string> words;
    std::stringstream ss;
    std::string word;

    for (auto line : lines) {
        for (int i = 0; i < line.size(); i++) {
            if (this->isValidCharacter(line[i])) {
                // Push char to buffer
                ss << line[i];
            } else {
                // Push new word from buffer
                word = ss.str();
                if (!word.empty())
                    words.push_back(word);

                // Clear buffer
                ss.str(std::string());
            }
        }
    }

    return words;
}

bool Parser::isValidCharacter(char c) {
    return isalnum(c);
}