#include "compression/bdi.h"
#include <cassert>
#include <iostream>

void testSimpleCompression() {
    compression::BDI bdi;
    std::vector<uint8_t> input = {
        1, 0, 0, 0,  // Base value
        2, 0, 0, 0   // Base + 1
    };
    
    auto compressed = bdi.compress(input);
    auto decompressed = bdi.decompress(compressed);
    
    assert(decompressed == input);
    std::cout << "Simple BDI compression test passed\n";
}

void testLargeDeltas() {
    compression::BDI bdi;
    std::vector<uint8_t> input = {
        0, 0, 0, 0,      // Base
        255, 255, 0, 0   // Large delta
    };
    
    auto compressed = bdi.compress(input);
    auto decompressed = bdi.decompress(compressed);
    
    assert(decompressed == input);
    std::cout << "Large delta BDI compression test passed\n";
}

int main() {
    testSimpleCompression();
    testLargeDeltas();
    
    std::cout << "All BDI tests passed!\n";
    return 0;
} 