#include "compression.hpp"
#include <bitset>
#include "log.hpp"

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

    LOG_D("Encoding " << number);

    while (true) {
        bytes.push_back(number % 128);

        if (number < 128)
            break;

        number /= 128;
    }

    std::reverse(bytes.begin(), bytes.end());

    // Flip first bit of last byte
    bytes[bytes.size() - 1] += 128;

    for (auto byte : bytes) {
        LOG_D(std::bitset<8>(byte));
    }

    return bytes;
}

std::vector<uint32_t> Compression::decode(std::vector<uint8_t> bytestream) {
    std::vector<uint32_t> numbers;
    uint32_t n = 0;

    LOG_D("Decoding");
    for (auto byte : bytestream) {
        LOG_D(std::bitset<8>(byte));
        if (byte < 128) {
            n = 128*n + byte;
        } else {
            n = 128*n + (byte - 128);
            LOG_D("Decoded: " << n);
            numbers.push_back(n);
            n = 0;
        }
    }

    return numbers;
}
