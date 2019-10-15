#include "parser.hpp"

Parser::Parser(std::string path) : path(path), eof(false) {
    this->infile.open(path);
}

std::pair<std::string, std::vector<std::pair<std::string, int>>> Parser::parseDocument() {
    auto lines = this->parseDocumentLines();
    auto headers = this->parseDocumentHeaders(lines);
    auto content = this->parseDocumentContent(lines);
    auto url = this->parseDocumentURL(headers);
    auto terms = this->parseDocumentTerms(content);
    auto frequencies = this->calculateFrequencies(terms);
    return std::make_pair(url, frequencies);
}

std::vector<std::string> Parser::parseDocumentLines() {
    std::vector<std::string> lines;
    std::string line;

    while (true) {
        line = this->parseLine();

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

std::string Parser::parseLine() {
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

std::vector<std::string> Parser::parseDocumentHeaders(std::vector<std::string> lines) {
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

std::vector<std::string> Parser::parseDocumentContent(std::vector<std::string> lines) {
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

std::string Parser::parseDocumentURL(std::vector<std::string> headers) {
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

std::vector<std::string> Parser::parseDocumentTerms(std::vector<std::string> lines) {
    std::vector<std::string> terms;
    std::stringstream ss;
    std::string term;

    for (auto line : lines) {
        // LOG_D("Line: " << line);
        for (int i = 0; i < line.size(); i++) {
            if (this->isValidCharacter(line[i])) {
                // Push char to buffer
                // Convert char to lowercase
                if ('A' <= line[i] && line[i] <= 'Z')
                    ss << (char)(line[i] + 32);
                else
                    ss << line[i];

            } else {
                // Push new term from buffer
                term = ss.str();
                if (this->isValidTerm(term)) {
                    // LOG_D("Term: " << term);
                    terms.push_back(term);
                }

                // Clear buffer
                ss.str(std::string());
            }
        }

        // Push new term from buffer whenever line has finished
        term = ss.str();
        if (this->isValidTerm(term))
            terms.push_back(term);

        // Clear buffer
        ss.str(std::string());
    }

    return terms;
}

bool Parser::isValidCharacter(char c) {
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

bool Parser::isValidTerm(std::string term) {
    return !term.empty() && term.size() <= MAX_TERM_SIZE && this->isTermNotOnlyDigits(term);
}

bool Parser::isTermNotOnlyDigits(std::string term) {
    for (char c : term) {
        if ('a' <= c && c <= 'z')
            return true;
    }

    return false;
}

std::vector<std::pair<std::string, int>> Parser::calculateFrequencies(std::vector<std::string> terms) {
    std::vector<std::pair<std::string, int>> frequencies;
    std::unordered_map<std::string, int> wordCount;

    for (auto term : terms) {
        if (wordCount.count(term) == 0)
            wordCount[term] = 1;
        else
            wordCount[term]++;
    }

    for (auto pair : wordCount) {
        frequencies.push_back(pair);
    }

    return frequencies;
}
