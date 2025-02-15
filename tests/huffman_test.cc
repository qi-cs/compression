#include <iostream>
#include <string>
#include "compression/huffman.h"  // Assuming your Huffman implementation is in this file

int main() {
    // Test case 1: Simple string compression
    std::string test_string = "hello world! this is a test string for huffman compression";
    std::cout << "Original string: " << test_string << std::endl;
    std::cout << "Original size: " << test_string.size() << " bytes" << std::endl;

    // Compress
    HuffmanCompression huffman;
    std::string compressed = huffman.compress(test_string);
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    std::cout << "Compressed string: " << compressed << std::endl;

    // Decompress
    std::string decompressed = huffman.decompress(compressed);
    std::cout << "Decompressed string: " << decompressed << std::endl;

    // Verify
    if (test_string == decompressed) {
        std::cout << "Test PASSED: Original and decompressed strings match!" << std::endl;
    } else {
        std::cout << "Test FAILED: Strings don't match!" << std::endl;
    }

    return 0;
}