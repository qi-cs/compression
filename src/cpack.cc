#include "compression/cpack.h"
#include <algorithm>
#include <stdexcept>

namespace compression {

CPack::CPack() = default;

std::vector<uint8_t> CPack::compress(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> compressed;
    
    // Process data in blocks of 4 bytes
    for (size_t i = 0; i < data.size(); i += 4) {
        size_t remaining = std::min(size_t(4), data.size() - i);
        auto block = compressBlock(data, i, remaining);
        
        // Add pattern byte
        compressed.push_back(block.pattern);
        // Add compressed data
        compressed.insert(compressed.end(), block.data.begin(), block.data.end());
    }
    
    return compressed;
}

std::vector<uint8_t> CPack::decompress(const std::vector<uint8_t>& compressed_data) {
    std::vector<uint8_t> decompressed;
    size_t offset = 0;
    
    while (offset < compressed_data.size()) {
        auto block = decompressBlock(compressed_data, offset);
        decompressed.insert(decompressed.end(), block.begin(), block.end());
    }
    
    return decompressed;
}

CPack::CompressedBlock CPack::compressBlock(const std::vector<uint8_t>& data, size_t offset, size_t size) {
    CompressedBlock block;
    
    // Check if all bytes in block are zero
    bool all_zero = true;
    for (size_t i = 0; i < size; ++i) {
        if (data[offset + i] != 0) {
            all_zero = false;
            break;
        }
    }
    
    if (all_zero) {
        block.pattern = ZERO_PATTERN;
        // No need to store data for zero block
    } else {
        block.pattern = BASE_PATTERN;
        // Store the actual data
        block.data.insert(block.data.end(), 
                         data.begin() + offset,
                         data.begin() + offset + size);
    }
    
    return block;
}

std::vector<uint8_t> CPack::decompressBlock(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size()) {
        throw std::runtime_error("Invalid compressed data");
    }
    
    uint8_t pattern = data[offset++];
    std::vector<uint8_t> block;
    
    if (pattern == ZERO_PATTERN) {
        // Zero block
        block = std::vector<uint8_t>(4, 0);
    } else if (pattern == BASE_PATTERN) {
        // Copy the next 4 bytes
        for (int i = 0; i < 4 && offset < data.size(); ++i) {
            block.push_back(data[offset++]);
        }
    } else {
        throw std::runtime_error("Invalid pattern byte");
    }
    
    return block;
}

} // namespace compression 