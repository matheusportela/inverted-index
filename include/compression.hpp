/**
    Variable byte compression.

    Convert each uint32_t number to a vector of uint8_t numbers.

    This algorithm uses the most-significant bit (MSB) of every byte in the
    compressed format as a continuation bit and the remaining 7 bits are used to
    store the actual information (payload).

    A continuation bit 1 means this is the last byte of the compressed
    representation. Otherwise, 0 means the following byte is also encoding the same number.

    When decoding a compressed number, the method should read all bytes until the continuation bit is set to 1. Then, it simply needs to remove the continuation bit and to concatenate all 7-bits payloads to restore the decoded information.

    Examples:
        5 -> 10000101
        132 -> 00000001 10000100
        78653 -> 00000100 01100110 10111101

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
    // Encode a list of uint32_t numbers into a list of uint8_t
    // @param numbers - List of uint32_t numbers
    // @return List of uint8_t encoded using variable byte compression
    static std::vector<uint8_t> encode(std::vector<uint32_t> numbers);

    // Encode one uint32_t numbers into a list of uint8_t
    // @param number - uint32_t number to be encoded
    // @return List of uint8_t encoded using variable byte compression
    static std::vector<uint8_t> encodeNumber(uint32_t number);

    // Decode a list of uint8_t into their original uint32_t numbers
    // @param bytestream - Encoded numbers in variable byte format
    // @return List of uint32_t numbers
    static std::vector<uint32_t> decode(std::vector<uint8_t> bytestream);
};

#endif // COMPRESSION_HPP
