#ifndef CPACK_H
#define CPACK_H

#include "compression_base.h"
#include "common.h"
#include <vector>
#include <cstdint>
#include <string>
#include <iomanip>

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

    std::string
    getPatternName(uint8_t pattern) const {
        switch (pattern) {
            case ZERO_PATTERN:      return "ZERO_PATTERN";
            case NONE_MATCH:        return "NONE_MATCH";
            case MATCH_DICT: return "MATCH_DICT";
            case PARTIAL_MATCH_2B: return "PARTIAL_MATCH_2B";
            case ZERO_UNMATCH: return "ZERO_UNMATCH";
            case PARTIAL_MATCH_3B: return "PARTIAL_MATCH_3B";
        }
        return "UNKNOWN";
    }

    struct Compressed2Word {
        uint8_t     pattern = 0;
        uint32_t    dict_index = 0;
        std::vector<uint8_t> unmatch_data;
    };

    // print out pattern and dict_index and also every element in unmatch_data
    std::string printCompressed2Word(const Compressed2Word& block) const {
        std::string result = std::string(getPatternName(block.pattern)) + " " + std::to_string(block.dict_index) + " ";
        for (const auto& data : block.unmatch_data) {
            std::stringstream ss;
            ss << std::hex << static_cast<int>(data);
            result += ss.str() + " ";
        }
        return result;
    }

    uint32_t getCompBlkSize(uint8_t pattern) const {
        // this is the size of pattern + dict_index + unmatch_data
        // like partial match 2B, it has 2B unmatch data
        // like zero unmatch, it has 1B unmatch data
        // zero pattern and none match does not have unmatch data
        if (pattern == ZERO_PATTERN) {
            return 1 + 4;
        } else if (pattern == NONE_MATCH) {
            return 1 + 4 + 64;
        } else if (pattern == MATCH_DICT) {
            return 1 + 4;
        } else if (pattern == PARTIAL_MATCH_2B) {
            return 1 + 4 + 2;
        } else if (pattern == ZERO_UNMATCH) {
            return 1 + 4 + 1;
        } else if (pattern == PARTIAL_MATCH_3B) {
            return 1 + 4 + 2;
        }
        return 0;
    }

    void setFreeze(bool freeze) {
        dict_.setFreeze(freeze);
    }

    void printDict() const {
        dict_.print();
    }

private:
    Compressed2Word compress2Word(const std::vector<uint8_t>& data, size_t offset, size_t size);
    std::vector<uint8_t> decompress2Word(const std::vector<uint8_t>& data, size_t& offset);

    Compressed2Word compress2Word(const uint32_t& data);

    Dictionary dict_;
};

} // namespace compression

#endif // CPACK_H 