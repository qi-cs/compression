#include "compression/fpc.h"
#include <algorithm>
#include <stdexcept>

namespace compression {

FPC::FPC() = default;

std::vector<uint8_t> FPC::compress(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> compressed;
    
    for (size_t i = 0; i < data.size(); i += 4) {  // Process 4-byte blocks
        auto block = compressBlock(data, i);
        
        // Store pattern
        compressed.push_back(block.pattern);
        
        // Store compressed data
        compressed.insert(compressed.end(), block.data.begin(), block.data.end());
    }
    
    return compressed;
}

std::vector<uint8_t> FPC::decompress(const std::vector<uint8_t>& compressed_data) {
    std::vector<uint8_t> decompressed;
    size_t offset = 0;
    
    while (offset < compressed_data.size()) {
        auto block = decompressBlock(compressed_data, offset);
        decompressed.insert(decompressed.end(), block.begin(), block.end());
    }
    
    return decompressed;
}

FPC::CompressedBlock FPC::compressBlock(const std::vector<uint8_t>& data, size_t offset) {
    CompressedBlock block;
    block.pattern = detectPattern(data, offset);
    
    size_t remaining = std::min(size_t(4), data.size() - offset);
    
    switch (block.pattern) {
        case ZERO:
            // No additional data needed
            break;
            
        case REPEATED_ZERO:
            // Store zero count
            block.data.push_back(remaining);
            break;
            
        case REPEATED_VALUE:
            // Store the repeated value
            block.data.push_back(data[offset]);
            break;
            
        case HALF_PRECISION:
            // Store 2 bytes instead of 4
            block.data.insert(block.data.end(),
                            data.begin() + offset,
                            data.begin() + offset + 2);
            break;
            
        case UNCOMPRESSED:
        default:
            // Store uncompressed data
            block.data.insert(block.data.end(),
                            data.begin() + offset,
                            data.begin() + offset + remaining);
            break;
    }
    
    return block;
}

std::vector<uint8_t> FPC::decompressBlock(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size()) {
        throw std::runtime_error("Invalid compressed data");
    }
    
    uint8_t pattern = data[offset++];
    std::vector<uint8_t> block;
    
    switch (pattern) {
        case ZERO:
            block = std::vector<uint8_t>(4, 0);
            break;
            
        case REPEATED_ZERO:
            block = std::vector<uint8_t>(data[offset++], 0);
            break;
            
        case REPEATED_VALUE: {
            uint8_t value = data[offset++];
            block = std::vector<uint8_t>(4, value);
            break;
        }
            
        case HALF_PRECISION: {
            block.push_back(data[offset++]);
            block.push_back(data[offset++]);
            block.push_back(0);
            block.push_back(0);
            break;
        }
            
        case UNCOMPRESSED:
        default:
            for (int i = 0; i < 4 && offset < data.size(); ++i) {
                block.push_back(data[offset++]);
            }
            break;
    }
    
    return block;
}

uint8_t FPC::detectPattern(const std::vector<uint8_t>& data, size_t offset) {
    size_t remaining = std::min(size_t(4), data.size() - offset);
    
    // Check for zero pattern
    bool all_zero = true;
    for (size_t i = 0; i < remaining; ++i) {
        if (data[offset + i] != 0) {
            all_zero = false;
            break;
        }
    }
    if (all_zero) return ZERO;
    
    // Check for repeated value
    bool all_same = true;
    for (size_t i = 1; i < remaining; ++i) {
        if (data[offset + i] != data[offset]) {
            all_same = false;
            break;
        }
    }
    if (all_same) return REPEATED_VALUE;
    
    // Check if upper half is all zeros (candidate for half precision)
    bool upper_zero = true;
    for (size_t i = 2; i < remaining; ++i) {
        if (data[offset + i] != 0) {
            upper_zero = false;
            break;
        }
    }
    if (upper_zero) return HALF_PRECISION;
    
    return UNCOMPRESSED;
}

} // namespace compression 