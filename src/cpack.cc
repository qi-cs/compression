#include "compression/cpack.h"
#include <algorithm>
#include <stdexcept>

namespace compression {

CPack::CPack() : dict_(1024) {
}

std::vector<uint8_t> CPack::compress(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> compressed;
    
    // Process data in blocks of 64 bytes
    for (size_t i = 0; i < data.size(); i += 64 ) {
        size_t remaining = std::min(size_t(64), data.size() - i);
        auto block = compressBlock(data, i, remaining);
        
        // Add pattern byte
        compressed.push_back(block.pattern);

        for (int i = 0; i < 4; i++) {
            compressed.push_back((block.dict_index >> (i * 8)) & 0xFF);
        }
        
        compressed.push_back(block.dict_index);
        // Add compressed data
        compressed.insert(compressed.end(), block.unmatch_data.begin(), block.unmatch_data.end());
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


CPack::CompressedBlock CPack::compress2Word(const uint32_t& data) {
    CompressedBlock block;
    if (data == 0) {
        block.pattern = ZERO_PATTERN;
        return block;
    }

    if (data&0x000000FF == 0) {
        block.pattern = ZERO_UNMATCH;
        block.unmatch_data.push_back(data & 0xFF);
        return block;
    }

    auto dict_entry = dict_.find_exact(data);
    if (dict_entry) {
        block.pattern = MATCH_DICT;
        block.dict_index = data;
        return block;
    }

    dict_entry = dict_.find_24bit(data);
    if (dict_entry) {
        block.pattern = PARTIAL_MATCH_3B;
        block.dict_index = data;
        block.unmatch_data.push_back(data & 0xFF);
        return block;
    }

    dict_entry = dict_.find_16bit(data);
    if (dict_entry) {
        block.pattern = PARTIAL_MATCH_2B;
        block.dict_index = data;
        block.unmatch_data.push_back((data >> 0) & 0xFF);
        block.unmatch_data.push_back((data >> 8) & 0xFF);
        return block;
    }   


    block.pattern = NONE_MATCH;

    for (int i = 0; i < 4; i++) {
        block.unmatch_data.push_back((data >> (i * 8)) & 0xFF);
    }

    return block;
}

CPack::CompressedBlock CPack::compressBlock(const std::vector<uint8_t>& data, size_t offset, size_t size) {
    CompressedBlock block;
    uint32_t doublewords = 0;  
    // Check if all bytes in block are zero
    bool all_zero = true;

    for (size_t i = 0; i < size; ++i) {
        if (data[offset + i] != 0) {
            all_zero = false;
        }
        doublewords |= data[offset + i] << (i * 8);
    }

    block = compress2Word(doublewords);

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
    } else if (pattern == NONE_MATCH) {
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