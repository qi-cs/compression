#include "compression/bdi.h"
#include <algorithm>
#include <stdexcept>

namespace compression {

BDI::BDI() = default;

std::vector<uint8_t> BDI::compress(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> compressed;
    
    for (size_t i = 0; i < data.size(); i += 64) {  // Process 64-byte blocks
        auto block = compressBlock(data, i);
        
        // Store encoding
        compressed.push_back(block.encoding);
        
        uint8_t base_size = getBaseSize(block.encoding);

        if (block.encoding != UNCOMPRESSED) {
            // shift base value in to compressed data
            for (size_t j = 0; j < base_size; ++j) {
                compressed.push_back((block.base >> (j * 8)) & 0xFF);
            }
            // Store deltas
            compressed.insert(compressed.end(), block.deltas.begin(), block.deltas.end());
        } else {
            // Store uncompressed data
            compressed.insert(compressed.end(), 
                            data.begin() + i,
                            data.begin() + std::min(i + 64, data.size()));
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

BDI::CompressedBlock
BDI::compressBlkEncode(const std::vector<uint8_t>& data, size_t offset, uint8_t encoding) {
    uint8_t base_size = getBaseSize(encoding);
    uint8_t delta_size = getDeltaSize(encoding);
    int64_t delta_limit_max = getDeltaLimitMax(encoding);
    int64_t delta_limit_min = getDeltaLimitMin(encoding);

    CompressedBlock block;

    uint64_t base = findBase(data, offset, base_size);

    size_t block_size = std::min(size_t(64), data.size() - offset);

    bool compressible = true;

    std::vector<uint8_t> deltas;
    for (size_t i = 0; i < block_size; i += base_size) {
        int64_t value;
        if (base_size == 8) {
            value = getValue<uint64_t>(data, offset, i);
        } else if (base_size == 4) {
            value = getValue<uint32_t>(data, offset, i);
        } else if (base_size == 2) {
            value = getValue<uint16_t>(data, offset, i);
        }

        int64_t delta = value - base;
        if (delta < delta_limit_min || delta > delta_limit_max) {
            compressible = false;
            //printf("base %0#lx value %0#lx base_size: %d, delta_size: %d, delta: %0#lx false delta_limit_min: %0#lx delta_limit_max: %0#lx delta < delta_limit_min: %d delta > delta_limit_max: %d\n",
            //base, value, base_size, delta_size, delta, delta_limit_min, delta_limit_max, delta < delta_limit_min, delta > delta_limit_max);
            break;
        }

        for (int i = 0; i < delta_size; i++) {
            deltas.push_back((delta >> (i * 8)) & 0xFF);
        }
    }

    if (compressible) {
        block.encoding = encoding;
        block.base = base;
        block.deltas = deltas;
    } else {
        block.encoding = UNCOMPRESSED;
    }

    return block;
}


BDI::CompressedBlock BDI::compressBlock(const std::vector<uint8_t>& data, size_t offset) {
    CompressedBlock block;

    bool compressible = false;
    for (uint8_t i = BASE8_DELTA1; i <= BASE8_DELTA4; i++) {
        CompressedBlock tmp_block = compressBlkEncode(data, offset, i);
        //printf("encoding: %d\n", tmp_block.encoding);
        if (tmp_block.encoding != UNCOMPRESSED) {
            block = tmp_block;
            compressible = true;
            break;
        }
    }

    if (!compressible) {
        block.encoding = UNCOMPRESSED;
        block.deltas = std::vector<uint8_t>(data.begin() + offset, data.begin() + offset + 64);
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
        for (int i = 0; i < 64 && offset < data.size(); ++i) {
            block.push_back(data[offset++]);
        }
    } else {
        uint8_t base_size = getBaseSize(encoding);
        uint8_t delta_size = getDeltaSize(encoding);

        //printf("base_size: %d, delta_size: %d\n", base_size, delta_size);

        // Read base value
        uint64_t base = 0;
        for (size_t i = 0; i < base_size; ++i) {
            base |= static_cast<uint64_t>(data[offset++]) << (i * 8);
        }
        
        // Mask the base value according to its size
        if (base_size == 2) {
            base &= 0xFFFF;  // 16-bit mask
        } else if (base_size == 4) {
            base &= 0xFFFFFFFF;  // 32-bit mask
        }

        for (size_t i = 0; i < 64; i += base_size) {
            uint64_t value = base;
            if (delta_size == 1) {
                value += static_cast<int8_t>(data[offset++]);
            } else if (delta_size == 2) {
                value += static_cast<int16_t>(data[offset] | (data[offset + 1] << 8));
                offset += 2;
            } else if (delta_size == 4) {
                value += static_cast<int32_t>(data[offset] | 
                                            (data[offset + 1] << 8) | 
                                            (data[offset + 2] << 16) | 
                                            (data[offset + 3] << 24));
                offset += 4;
            }

            // Mask the result according to base_size before storing
            if (base_size == 2) {
                value &= 0xFFFF;  // Keep only lower 16 bits
            } else if (base_size == 4) {
                value &= 0xFFFFFFFF;  // Keep only lower 32 bits
            }

            // Store reconstructed value
            for (size_t j = 0; j < base_size; ++j) {
                block.push_back((value >> (j * 8)) & 0xFF);
            }
        }
    }
    
    return block;
}

uint64_t BDI::findBase(const std::vector<uint8_t>& data, size_t offset, uint8_t base_size) {
    uint64_t base = 0;
    size_t bytes_to_copy = std::min(size_t(base_size), data.size() - offset);
    std::copy(data.begin() + offset,
             data.begin() + offset + bytes_to_copy,
             reinterpret_cast<uint8_t*>(&base));
    return base;
}

} // namespace compression 