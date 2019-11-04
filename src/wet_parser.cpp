#include "wet_parser.hpp"

WETParser::WETParser(std::string path) : path(path), eof(false) {
    this->infile.open(path);
}

std::tuple<std::string, std::vector<std::pair<std::string, int>>, uint64_t, uint64_t> WETParser::parseDocument() {
    auto document_begin = this->currentPosition;
    LOG_D("Document start: " << document_begin);
    auto lines = this->parseDocumentLines();
    auto document_end = this->currentPosition;
    auto document_length = document_end - document_begin;
    LOG_D("Document end: " << document_end);
    auto headers = this->parseDocumentHeaders(lines);
    auto content = this->parseDocumentContent(lines);
    auto url = this->parseDocumentURL(headers);
    auto terms = this->parseDocumentTerms(content);
    auto frequencies = this->calculateFrequencies(terms);
    return std::make_tuple(url, frequencies, document_begin, document_length);
}

std::vector<std::string> WETParser::parseDocumentLines() {
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

std::string WETParser::parseLine() {
    std::stringstream ss;
    std::string line;

    // Read line from input stream char by char
    char c;
    while (this->infile.get(c)) {
        this->currentPosition++;

        if (c == '\n')
            break;

        ss << c;
    }

    line = ss.str();

    // Check for EOF
    if (this->infile.eof())
        this->setEOF();

    // Sanitize line
    if (line[line.size() - 1] == '\r')
        line.pop_back();

    return line;
}

void WETParser::setEOF() {
    this->eof = true;
}

bool WETParser::isEOF() {
    return this->eof;
}

bool WETParser::isDocumentBegin(std::string line) {
    return line == "WARC/1.0";
}

std::vector<std::string> WETParser::parseDocumentHeaders(std::vector<std::string> lines) {
    std::vector<std::string> headers;

    for (auto line : lines) {
        headers.push_back(line);

        if (this->isLastHeader(line))
            break;
    }

    return headers;
}

bool WETParser::isLastHeader(std::string line) {
    return line.rfind("Content-Length: ", 0) == 0;
}

std::vector<std::string> WETParser::parseDocumentContent(std::vector<std::string> lines) {
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

std::string WETParser::parseDocumentURL(std::vector<std::string> headers) {
    std::string url;

    for (auto header : headers) {
        if (this->isURLHeader(header))
            url = header.substr(std::string("WARC-Target-URI: ").size());
    }

    return url;
}

bool WETParser::isURLHeader(std::string line) {
    return line.rfind("WARC-Target-URI: ", 0) == 0;
}

std::vector<std::string> WETParser::parseDocumentTerms(std::vector<std::string> lines) {
    std::vector<std::string> terms;
    std::stringstream ss;
    std::string term;

    for (auto line : lines) {
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
                if (this->isValidTerm(term))
                    terms.push_back(term);

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

bool WETParser::isValidCharacter(char c) {
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

bool WETParser::isValidTerm(std::string term) {
    return !term.empty() && term.size() <= MAX_TERM_SIZE && this->isTermNotOnlyDigits(term);
}

bool WETParser::isTermNotOnlyDigits(std::string term) {
    for (char c : term) {
        if ('a' <= c && c <= 'z')
            return true;
    }

    return false;
}

std::vector<std::pair<std::string, int>> WETParser::calculateFrequencies(std::vector<std::string> terms) {
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
