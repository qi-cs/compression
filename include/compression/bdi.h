#ifndef BDI_H
#define BDI_H

#include "compression_base.h"
#include <vector>
#include <cstdint>

namespace compression {

class BDI : public CompressionBase {
public:
    BDI();
    ~BDI() override = default;

    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) override;

private:
    static constexpr uint8_t UNCOMPRESSED = 0x00;
    static constexpr uint8_t BASE_1BYTE = 0x01;
    static constexpr uint8_t BASE_2BYTE = 0x02;
    static constexpr uint8_t BASE_4BYTE = 0x03;
    
    struct CompressedBlock {
        uint8_t encoding;
        uint32_t base;
        std::vector<uint8_t> deltas;
    };

    CompressedBlock compressBlock(const std::vector<uint8_t>& data, size_t offset);
    std::vector<uint8_t> decompressBlock(const std::vector<uint8_t>& data, size_t& offset);
    uint32_t findBase(const std::vector<uint8_t>& data, size_t offset);
};

} // namespace compression

#endif // BDI_H 