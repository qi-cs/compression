#ifndef FPC_H
#define FPC_H

#include "compression_base.h"
#include <vector>
#include <cstdint>

namespace compression {

class FPC : public CompressionBase {
public:
    FPC();
    ~FPC() override = default;

    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) override;

private:
    static constexpr uint8_t ZERO = 0x00;
    static constexpr uint8_t REPEATED_ZERO = 0x01;
    static constexpr uint8_t REPEATED_VALUE = 0x02;
    static constexpr uint8_t UNCOMPRESSED = 0x03;
    static constexpr uint8_t HALF_PRECISION = 0x04;
    
    struct CompressedBlock {
        uint8_t pattern;
        std::vector<uint8_t> data;
    };

    CompressedBlock compressBlock(const std::vector<uint8_t>& data, size_t offset);
    std::vector<uint8_t> decompressBlock(const std::vector<uint8_t>& data, size_t& offset);
    uint8_t detectPattern(const std::vector<uint8_t>& data, size_t offset);
};

} // namespace compression

#endif // FPC_H 