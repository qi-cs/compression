#ifndef CPACK_H
#define CPACK_H

#include "compression_base.h"
#include "common.h"
#include <vector>
#include <cstdint>

namespace compression {

class CPack : public CompressionBase {
public:
    CPack();
    ~CPack() override = default;

    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) override;

    /*
     * 00 - zzzz (00) zero pattern                              2-bit
     * 01 - xxxx (01)BBBB  none-match           B: real data    34-bit
     * 10 - mmmm (10)bbbb  match-dictionary     b: dict index   6-bit
     * 1100 - mmxx (1100)bbbbBB partial-match index+unmatchdata 24-bit
     * 1101 - zzzx (1101)B zero+unmatchdata                     12-bit
     * 1110 - mmmx (1110)bbbbB partial-match index+unmatchdata  16-bit
    */
    static constexpr uint8_t ZERO_PATTERN       = 0x00;
    static constexpr uint8_t NONE_MATCH         = 0x01;
    static constexpr uint8_t MATCH_DICT         = 0x02;
    static constexpr uint8_t PARTIAL_MATCH_2B   = 0x03;
    static constexpr uint8_t ZERO_UNMATCH       = 0x04;
    static constexpr uint8_t PARTIAL_MATCH_3B   = 0x05;

    
    struct CompressedBlock {
        uint8_t     pattern = 0;
        uint32_t    dict_index = 0;
        std::vector<uint8_t> unmatch_data;
    };

private:
    CompressedBlock compressBlock(const std::vector<uint8_t>& data, size_t offset, size_t size);
    std::vector<uint8_t> decompressBlock(const std::vector<uint8_t>& data, size_t& offset);

    CompressedBlock compress2Word(const uint32_t& data);

    Dictionary dict_;
};

} // namespace compression

#endif // CPACK_H 