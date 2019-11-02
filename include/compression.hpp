/**
    Variable byte encoding

    References:
    - https://nlp.stanford.edu/IR-book/html/htmledition/variable-byte-codes-1.html
*/

#ifndef COMPRESSION_HPP
#define COMPRESSION_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

class Compression {
  public:
    static std::vector<uint8_t> encode(std::vector<uint32_t> numbers);
    static std::vector<uint8_t> encodeNumber(uint32_t number);
    static std::vector<uint32_t> decode(std::vector<uint8_t> bytestream);
};

#endif // COMPRESSION_HPP
