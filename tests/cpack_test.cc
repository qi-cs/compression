#include "compression/cpack.h"
#include <cassert>
#include <iostream>

void testZeroCompression() {
    compression::CPack cpack;
    std::vector<uint8_t> input(8, 0);  // 8 zero bytes
    
    auto compressed = cpack.compress(input);
    auto decompressed = cpack.decompress(compressed);
    
    assert(decompressed == input);
    std::cout << "Zero compression test passed\n";
}

void testMixedDataCompression() {
    compression::CPack cpack;
    std::vector<uint8_t> input = {1, 2, 3, 4, 0, 0, 0, 0};
    
    auto compressed = cpack.compress(input);
    auto decompressed = cpack.decompress(compressed);
    
    assert(decompressed == input);
    std::cout << "Mixed data compression test passed\n";
}

int main() {
    testZeroCompression();
    testMixedDataCompression();
    
    std::cout << "All tests passed!\n";
    return 0;
} 