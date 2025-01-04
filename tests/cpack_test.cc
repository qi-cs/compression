#include "compression/cpack.h"
#include <cassert>
#include <iostream>

void testZeroCompression() {
    compression::CPack cpack;
    std::vector<uint8_t> input(64, 0);  // 64 zero bytes
    
    auto compressed = cpack.compress(input);
    //cpack.print_bytes(compressed, 16);
    auto decompressed = cpack.decompress(compressed);
    //cpack.print_bytes(decompressed, 16);
    
    assert(decompressed == input);
    std::cout << "Zero compression test passed\n";
}

void generateDict(std::vector<uint32_t>& dict) {
    for (size_t i = 0; i < 32; i++) {
        dict.push_back(rand() % 0xFFFFFF);
        //printf("dict[%zu]: %08X\n", i, dict[i]);
    }
}

void generatePattern2word(std::vector<uint8_t>& data, std::vector<uint32_t>& dict) {
    uint8_t pattern = rand() % 3;
    if (pattern == 0) {
        data.resize(4,0);
    } else if (pattern == 1) {
        // random data
        for (size_t i = 0; i < 4; i++) {    
            data.push_back(rand() % 0xFF);
        }
    } else if (pattern == 2) {
        // match dict
        uint32_t random_seed = rand() % dict.size();
        for (size_t i = 0; i < 4; i++) {    
            data.push_back(dict[random_seed] >> (i * 8));
        }
    } else if (pattern == 3) {
        // partial match 2B
        uint32_t random_seed = rand() % dict.size();
        for (size_t i = 0; i < 4; i++) {    
            data.push_back(dict[random_seed] >> (i * 8));
            if (i == 0) {
                data[0] = data[0] | rand() % 0xFF;
            } else if (i == 1) {
                data[1] = data[1] | rand() % 0xFF;
            }
        }
    } else if (pattern == 4) {
        // zero unmatch
        for (size_t i = 0; i < 3; i++) {    
            data.push_back(0);
        }
        data.push_back(rand() % 0xFF);
    } else if (pattern == 5) {
        // partial match 3B
        uint32_t random_seed = rand() % dict.size();
        for (size_t i = 0; i < 4; i++) {    
            data.push_back(dict[random_seed] >> (i * 8));
            if (i == 0) {
                data[0] = data[0] | rand() % 0xFF;
            }
        }
    }
}

void generateBlock(std::vector<uint8_t>& data, std::vector<uint32_t>& dict) {
    std::vector<uint8_t> tmpdata;
    // cpack compresses at 4-byte level
    for (size_t i = 0; i < 16; i++) { 
        generatePattern2word(tmpdata,dict);   
        data.insert(data.end(), tmpdata.begin(), tmpdata.end());
        tmpdata.clear();
    }
}

void testMixedDataCompression() {
    compression::CPack cpack;

    
    std::vector<uint32_t> dict;

    std::vector<uint8_t> input;

    generateDict(dict);

    for (size_t i = 0; i < 100; i++) {
        generateBlock(input, dict);

        //cpack.print_bytes(input, 16);

        auto compressed = cpack.compress(input);
        //cpack.print_bytes(compressed, 16);
        auto decompressed = cpack.decompress(compressed);
        //cpack.print_bytes(decompressed, 16);
        assert(decompressed == input);

        //printf("compress %ld passed.\n", i);
    }
    //cpack.printDict();
    std::cout << "Mixed data compression test passed\n";
}

int main() {
    testZeroCompression();

    testMixedDataCompression();
    
    std::cout << "All tests passed!\n";

    return 0;
} 