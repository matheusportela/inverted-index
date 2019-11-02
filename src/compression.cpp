#include "compression.hpp"

std::vector<uint8_t> Compression::encode(std::vector<uint32_t> numbers) {
    std::vector<uint8_t> bytestream;

    for (auto number : numbers) {
        std::vector<uint8_t> bytes = encodeNumber(number);

        for (auto byte : bytes) {
            bytestream.push_back(byte);
        }
    }

    return bytestream;
}

std::vector<uint8_t> Compression::encodeNumber(uint32_t number) {
    std::vector<uint8_t> bytes;

    while (true) {
        bytes.push_back(number % 128);

        if (number < 128)
            break;

        number /= 128;
    }

    std::reverse(bytes.begin(), bytes.end());

    // Flip first bit of last byte
    bytes[bytes.size() - 1] += 128;

    return bytes;
}

std::vector<uint32_t> Compression::decode(std::vector<uint8_t> bytestream) {
    std::vector<uint32_t> numbers;
    uint32_t n = 0;

    for (auto byte : bytestream) {
        if (byte < 128) {
            n = 128*n + byte;
        } else {
            n = 128*n + (byte - 128);
            numbers.push_back(n);
            n = 0;
        }
    }

    return numbers;
}
