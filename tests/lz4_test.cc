// tests/lz4_test.cc
#include "compression/lz4.h"
#include <iostream>
#include <sstream>
#include <cassert>

void testVectorCompression() {
    LZ4Compressor compressor;
    std::vector<char> data = {'d','e','a', 'b', 'c', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd'};
    printf("data size: %ld\n", data.size());
    compressor.printOutput(data);
    std::vector<char> compressed = compressor.compress(data);
    printf("compressed size: %ld\n", compressed.size());
    compressor.printOutput(compressed);
    std::vector<char> decompressed = compressor.decompress(compressed);
    printf("decompressed size: %ld\n", decompressed.size());
    compressor.printOutput(decompressed);
    assert(data == decompressed);
    std::cout << "Vector compression test passed." << std::endl;
}

void testEmptyData() {
    LZ4Compressor compressor;
    std::vector<char> data;
    std::vector<char> compressed = compressor.compress(data);
    std::vector<char> decompressed = compressor.decompress(compressed);

    assert(data == decompressed);
    std::cout << "Empty data test passed." << std::endl;
}

void testSingleCharacter() {
    LZ4Compressor compressor;
    std::vector<char> data = {'x'};
    std::vector<char> compressed = compressor.compress(data);
    std::vector<char> decompressed = compressor.decompress(compressed);
    compressor.printOutput(decompressed);
    assert(data == decompressed);
    std::cout << "Single character test passed." << std::endl;
}

void testCharacterRepeat() {
    LZ4Compressor compressor;
    std::vector<char> data = {'x', 'x', 'x', 'x','x', 'x', 'x', 'x', 'x'};
    std::vector<char> compressed = compressor.compress(data);
    compressor.printOutput(compressed);
    std::vector<char> decompressed = compressor.decompress(compressed);
    compressor.printOutput(decompressed);
    assert(data == decompressed);
    std::cout << "Character repeat test passed." << std::endl;
}

int main() {
    testVectorCompression();
    testEmptyData();
    //testCharacterRepeat();
    testSingleCharacter();
    return 0;
}