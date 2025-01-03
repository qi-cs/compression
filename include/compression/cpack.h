#ifndef CPACK_H
#define CPACK_H

#include "compression_base.h"
#include <vector>
#include <cstdint>

namespace compression {

class CPack : public CompressionBase {
public:
    CPack();
    ~CPack() override = default;

    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) override;

private:
    static constexpr uint8_t ZERO_PATTERN = 0x00;
    static constexpr uint8_t BASE_PATTERN = 0x01;
    
    struct CompressedBlock {
        uint8_t pattern;
        std::vector<uint8_t> data;
    };

    CompressedBlock compressBlock(const std::vector<uint8_t>& data, size_t offset, size_t size);
    std::vector<uint8_t> decompressBlock(const std::vector<uint8_t>& data, size_t& offset);
};

} // namespace compression

#endif // CPACK_H 