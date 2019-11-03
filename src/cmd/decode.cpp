#include <bitset>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "compression.hpp"

int main(int argc, char* argv[]) {
    std::vector<uint8_t> numbers;

    if (argc == 1) {
        std::cout << "Usage: decode numbers..." << std::endl;
        return 1;
    }

    for (int i = 0; i < argc - 1; i++) {
        numbers.push_back(std::stoi(argv[i + 1]));
    }

    auto decompressed = Compression::decode(numbers);

    std::cout << "int:" << std::endl;
    for (int i = 0; i < decompressed.size(); i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        auto byte = decompressed[i];
        std::cout << std::setfill('0') << std::setw(3) << (unsigned int)byte << " ";
    }
    std::cout << std::endl << std::endl << std::endl;


    std::cout << "hex:" << std::endl;
    for (int i = 0; i < decompressed.size(); i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        auto byte = decompressed[i];
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)byte << " ";
    }
    std::cout << std::endl << std::endl << std::endl;

    std::cout << "bin:" << std::endl;
    for (int i = 0; i < decompressed.size(); i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        auto byte = decompressed[i];
        std::cout << std::bitset<8>(byte) << " ";
    }
    std::cout << std::endl << std::endl << std::endl;

    return 0;
}
