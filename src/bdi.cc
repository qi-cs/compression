#include "compression/bdi.h"
#include <algorithm>
#include <stdexcept>

namespace compression {

BDI::BDI() = default;

std::vector<uint8_t> BDI::compress(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> compressed;
    
    for (size_t i = 0; i < data.size(); i += 8) {  // Process 8-byte blocks
        auto block = compressBlock(data, i);
        
        // Store encoding
        compressed.push_back(block.encoding);
        
        // Store base value
        if (block.encoding != UNCOMPRESSED) {
            for (size_t j = 0; j < sizeof(block.base); ++j) {
                compressed.push_back((block.base >> (j * 8)) & 0xFF);
            }
            // Store deltas
            compressed.insert(compressed.end(), block.deltas.begin(), block.deltas.end());
        } else {
            // Store uncompressed data
            compressed.insert(compressed.end(), 
                            data.begin() + i,
                            data.begin() + std::min(i + 8, data.size()));
        }
    }
    
    return compressed;
}

std::vector<uint8_t> BDI::decompress(const std::vector<uint8_t>& compressed_data) {
    std::vector<uint8_t> decompressed;
    size_t offset = 0;
    
    while (offset < compressed_data.size()) {
        auto block = decompressBlock(compressed_data, offset);
        decompressed.insert(decompressed.end(), block.begin(), block.end());
    }
    
    return decompressed;
}

BDI::CompressedBlock BDI::compressBlock(const std::vector<uint8_t>& data, size_t offset) {
    CompressedBlock block;
    uint32_t base = findBase(data, offset);
    
    // Try different delta sizes
    std::vector<int8_t> deltas_1byte;
    std::vector<int16_t> deltas_2byte;
    bool can_use_1byte = true;
    bool can_use_2byte = true;
    
    size_t block_size = std::min(size_t(8), data.size() - offset);
    
    for (size_t i = 0; i < block_size; i += 4) {
        uint32_t value;
        std::copy(data.begin() + offset + i, 
                 data.begin() + offset + i + 4,
                 reinterpret_cast<uint8_t*>(&value));
                 
        int32_t delta = value - base;
        
        if (delta < INT8_MIN || delta > INT8_MAX) can_use_1byte = false;
        if (delta < INT16_MIN || delta > INT16_MAX) can_use_2byte = false;
        
        if (can_use_1byte) deltas_1byte.push_back(delta);
        if (can_use_2byte) deltas_2byte.push_back(delta);
    }
    
    if (can_use_1byte) {
        block.encoding = BASE_1BYTE;
        block.base = base;
        block.deltas = std::vector<uint8_t>(
            reinterpret_cast<uint8_t*>(deltas_1byte.data()),
            reinterpret_cast<uint8_t*>(deltas_1byte.data() + deltas_1byte.size())
        );
    } else if (can_use_2byte) {
        block.encoding = BASE_2BYTE;
        block.base = base;
        block.deltas = std::vector<uint8_t>(
            reinterpret_cast<uint8_t*>(deltas_2byte.data()),
            reinterpret_cast<uint8_t*>(deltas_2byte.data() + deltas_2byte.size())
        );
    } else {
        block.encoding = UNCOMPRESSED;
    }
    
    return block;
}

std::vector<uint8_t> BDI::decompressBlock(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size()) {
        throw std::runtime_error("Invalid compressed data");
    }
    
    uint8_t encoding = data[offset++];
    std::vector<uint8_t> block;
    
    if (encoding == UNCOMPRESSED) {
        for (int i = 0; i < 8 && offset < data.size(); ++i) {
            block.push_back(data[offset++]);
        }
    } else {
        // Read base value
        uint32_t base = 0;
        for (size_t i = 0; i < sizeof(base); ++i) {
            base |= static_cast<uint32_t>(data[offset++]) << (i * 8);
        }
        
        // Read and apply deltas
        size_t delta_size = (encoding == BASE_1BYTE) ? 1 : 2;
        for (size_t i = 0; i < 8; i += 4) {
            uint32_t value = base;
            if (encoding == BASE_1BYTE) {
                value += static_cast<int8_t>(data[offset++]);
            } else {
                value += static_cast<int16_t>(data[offset] | (data[offset + 1] << 8));
                offset += 2;
            }
            
            // Store reconstructed value
            for (size_t j = 0; j < 4; ++j) {
                block.push_back((value >> (j * 8)) & 0xFF);
            }
        }
    }
    
    return block;
}

uint32_t BDI::findBase(const std::vector<uint8_t>& data, size_t offset) {
    // Simple implementation: use first 4-byte value as base
    uint32_t base = 0;
    size_t bytes_to_copy = std::min(size_t(4), data.size() - offset);
    std::copy(data.begin() + offset,
             data.begin() + offset + bytes_to_copy,
             reinterpret_cast<uint8_t*>(&base));
    return base;
}

} // namespace compression 