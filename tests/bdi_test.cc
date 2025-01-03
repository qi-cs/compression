#include "compression/bdi.h"
#include <cassert>
#include <iostream>
#include <random>

std::vector<uint8_t> generateTestInput(int base_size = 8, int delta_size = 2) {
    std::vector<uint8_t> input(64);  // 64 bytes total
    
    // Generate random base value
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    // Number of values that will fit in 64 bytes
    int num_values = 64 / base_size;
    
    // Fill first base_size bytes with random data
    for (int i = 0; i < base_size; i++) {
        input[i] = dis(gen);
    }
    
    // Copy base value to other positions and add delta
    for (int block = 1; block < num_values; block++) {
        // Copy the base value
        for (int byte = 0; byte < base_size; byte++) {
            input[block * base_size + byte] = input[byte];
        }
        
        // Add delta based on delta_size
        if (delta_size == 1) {
            // For 1-byte delta, increment the first byte
            input[block * base_size] += block;
        } else if (delta_size == 2) {
            // For 2-byte delta, increment the second byte
            input[block * base_size + 1] += block;
        }
    }
    
    return input;
}

void testSimpleCompression() {
    compression::BDI bdi;
    std::vector<uint8_t> input;
    std::vector<uint8_t> compressed;
    std::vector<uint8_t> decompressed;
    
    input = generateTestInput(8,1);
    //bdi.print_bytes(input, 8);
    compressed = bdi.compress(input);
    printf("compressed size: %ld %s\n", compressed.size(), bdi.getEncodingName(compressed[0]).c_str());
    //bdi.print_bytes(compressed, 8);
    decompressed = bdi.decompress(compressed);
    //printf("decompressed size: %ld\n", decompressed.size());
    //bdi.print_bytes(decompressed, 8);
    assert(decompressed == input);
    std::cout << "Simple BDI 8-1 compression test passed\n";

    input = generateTestInput(8,2);
    //bdi.print_bytes(input, 8);
    compressed = bdi.compress(input);
    printf("compressed size: %ld %s\n", compressed.size(), bdi.getEncodingName(compressed[0]).c_str());
    //bdi.print_bytes(compressed, 8);
    decompressed = bdi.decompress(compressed);
    //printf("decompressed size: %ld\n", decompressed.size());
    //bdi.print_bytes(decompressed, 8);
    assert(decompressed == input);
    std::cout << "Simple BDI 8-2 compression test passed\n";
    
    input = generateTestInput(4,1);
    //bdi.print_bytes(input, 4);
    compressed = bdi.compress(input);
    printf("compressed size: %ld %s\n", compressed.size(), bdi.getEncodingName(compressed[0]).c_str());
    //bdi.print_bytes(compressed, 4);
    decompressed = bdi.decompress(compressed);
    //printf("decompressed size: %ld\n", decompressed.size());
    //bdi.print_bytes(decompressed, 4);
    assert(decompressed == input);
    std::cout << "Simple BDI 4-1 compression test passed\n";

    input = generateTestInput(4,2);
    //bdi.print_bytes(input, 4);
    compressed = bdi.compress(input);
    printf("compressed size: %ld %s\n", compressed.size(), bdi.getEncodingName(compressed[0]).c_str());
    //bdi.print_bytes(compressed, 4);
    decompressed = bdi.decompress(compressed);
    //printf("decompressed size: %ld\n", decompressed.size());
    //bdi.print_bytes(decompressed, 4);
    assert(decompressed == input);
    std::cout << "Simple BDI 4-2 compression test passed\n";
}


int main() {
    testSimpleCompression();
    
    std::cout << "All BDI tests passed!\n";
    return 0;
} 