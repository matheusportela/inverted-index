#include <cctype>
#include <iomanip>
#include <iostream>
#include <tuple>

#include "query_engine.hpp"
#include "lexicon.hpp"
#include "log.hpp"

#include "compression.hpp"
#include <bitset>

int main(int argc, char* argv[]) {
    LOG_SET_INFO();

    if (argc != 3) {
        std::cout << "Usage: inspector start end" << std::endl;
        return 1;
    }

    std::string param1 = std::string(argv[1]);
    std::string param2 = std::string(argv[2]);

    uint64_t start = std::stoull(param1);
    uint64_t end = std::stoull(param2);
    auto bytes = end - start;

    std::cout << "start: " << start << std::endl;
    std::cout << "end: " << end << std::endl;
    std::cout << "bytes: " << bytes << std::endl;
    std::cout << std::endl << std::endl << std::endl;

    std::ifstream fd;
    fd.open("../tmp/index.txt", std::ofstream::in | std::ofstream::binary);
    fd.seekg(start);

    uint8_t buffer[bytes];
    fd.read((char*)buffer, bytes);

    fd.close();

    std::cout << "int:" << std::endl;
    for (int i = 0; i < bytes; i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        auto byte = buffer[i];
        std::cout << std::setfill('0') << std::setw(3) << (unsigned int)byte << " ";
    }
    std::cout << std::endl << std::endl << std::endl;


    std::cout << "hex:" << std::endl;
    for (int i = 0; i < bytes; i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        auto byte = buffer[i];
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)byte << " ";
    }
    std::cout << std::endl << std::endl << std::endl;

    std::cout << "bin:" << std::endl;
    for (int i = 0; i < bytes; i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        auto byte = buffer[i];
        std::cout << std::bitset<8>(byte) << " ";
    }
    std::cout << std::endl << std::endl << std::endl;

    return 0;
}
