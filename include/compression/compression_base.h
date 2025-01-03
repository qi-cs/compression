#ifndef COMPRESSION_BASE_H
#define COMPRESSION_BASE_H

#include <vector>
#include <cstdint>

namespace compression {

class CompressionBase {
public:
    virtual ~CompressionBase() = default;
    
    // Pure virtual functions that derived classes must implement
    virtual std::vector<uint8_t> compress(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) = 0;
    
    // Common utility functions can be added here
protected:
    CompressionBase() = default;
};

} // namespace compression

#endif // COMPRESSION_BASE_H 