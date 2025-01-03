#include "compression/fpc.h"
#include <cassert>
#include <iostream>

void testZeroPattern() {
    compression::FPC fpc;
    std::vector<uint8_t> input = {0, 0, 0, 0};
    
    auto compressed = fpc.compress(input);
    auto decompressed = fpc.decompress(compressed);
    
    assert(decompressed == input);
    std::cout << "Zero pattern FPC compression test passed\n";
}

void testRepeatedValue() {
    compression::FPC fpc;
    std::vector<uint8_t> input = {42, 42, 42, 42};
    
    auto compressed = fpc.compress(input);
    auto decompressed = fpc.decompress(compressed);
    
    assert(decompressed == input);
    std::cout << "Repeated value FPC compression test passed\n";
}

int main() {
    testZeroPattern();
    testRepeatedValue();
    
    std::cout << "All FPC tests passed!\n";
    return 0;
} 