#ifndef BDI_H
#define BDI_H

#include "compression_base.h"
#include <vector>
#include <cstdint>
#include <string>
#include "common.h"

namespace compression {

class BDI : public CompressionBase {
public:
    BDI();
    ~BDI() override = default;

    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) override;
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) override;

private:
    // change encoding according to compression length
    static constexpr uint8_t UNCOMPRESSED = 0b00000000;
    static constexpr uint8_t REPEAT       = 0b00000001;
    static constexpr uint8_t BASE8_DELTA1 = 0b00000010;
    static constexpr uint8_t BASE4_DELTA1 = 0b00000011;
    static constexpr uint8_t BASE8_DELTA2 = 0b00000100;
    static constexpr uint8_t BASE4_DELTA2 = 0b00000101;
    static constexpr uint8_t BASE2_DELTA1 = 0b00000110;
    static constexpr uint8_t BASE8_DELTA4 = 0b00000111;

    uint32_t compSize(uint8_t encoding) {
        switch (encoding) {
            case UNCOMPRESSED:
                return 64;
            case REPEAT:
                return 8;
            case BASE8_DELTA1:
                return 16;
            case BASE8_DELTA2:
                return 24;
            case BASE8_DELTA4:
                return 40;
            case BASE4_DELTA1:
                return 20;
            case BASE4_DELTA2:
                return 36;
            case BASE2_DELTA1:
                return 34;
            default:
                return 0;
        }   
    }


    uint8_t getBaseSize(uint8_t encoding) {
        if (encoding == BASE8_DELTA1 || encoding == BASE8_DELTA2 || encoding == BASE8_DELTA4) {
            return 8;
        } else if (encoding == BASE4_DELTA1 || encoding == BASE4_DELTA2) {
            return 4;
        } else if (encoding == BASE2_DELTA1) {
            return 2;
        }
        return 0;
    }

    uint8_t getDeltaSize(uint8_t encoding) {
        if (encoding == BASE8_DELTA1 || encoding == BASE4_DELTA1 || encoding == BASE2_DELTA1) {
            return 1;
        } else if (encoding == BASE8_DELTA2 || encoding == BASE4_DELTA2) {
            return 2;
        } else if (encoding == BASE8_DELTA4) {
            return 4;
        }
        return 0;
    }

    int64_t getDeltaLimitMax(uint8_t encoding) {
        if (encoding == BASE8_DELTA1 || encoding == BASE4_DELTA1 || encoding == BASE2_DELTA1) {
            return INT8_MAX;
        } else if (encoding == BASE8_DELTA2 || encoding == BASE4_DELTA2) {
            return INT16_MAX;
        } else if (encoding == BASE8_DELTA4) {
            return INT32_MAX;
        }
        return 0;
    }   

    int64_t getDeltaLimitMin(uint8_t encoding) {
        if (encoding == BASE8_DELTA1 || encoding == BASE4_DELTA1 || encoding == BASE2_DELTA1) {
            return INT8_MIN;
        } else if (encoding == BASE8_DELTA2 || encoding == BASE4_DELTA2) {
            return INT16_MIN;
        } else if (encoding == BASE8_DELTA4) {
            return INT32_MIN;
        }
        return 0;
    }

    struct CompressedBlock {
        uint8_t encoding;
        uint64_t base;
        std::vector<uint8_t> deltas;
    };

    CompressedBlock compressBlkEncode(const std::vector<uint8_t>& data, size_t offset, uint8_t encoding);
    CompressedBlock compressBlock(const std::vector<uint8_t>& data, size_t offset);
    std::vector<uint8_t> decompressBlock(const std::vector<uint8_t>& data, size_t& offset);
    uint64_t findBase(const std::vector<uint8_t>& data, size_t offset, uint8_t base_size);

public:

    std::string getEncodingName(uint8_t encoding) {
        switch (encoding) {
            case UNCOMPRESSED: return "UNCOMPRESSED";
            case REPEAT: return "REPEAT";
            case BASE8_DELTA1: return "BASE8_DELTA1";
            case BASE8_DELTA2: return "BASE8_DELTA2";
            case BASE8_DELTA4: return "BASE8_DELTA4";
            case BASE4_DELTA1: return "BASE4_DELTA1";
            case BASE4_DELTA2: return "BASE4_DELTA2";
            case BASE2_DELTA1: return "BASE2_DELTA1";
            default: return "UNKNOWN";
        }
    }   
};

} // namespace compression

#endif // BDI_H 