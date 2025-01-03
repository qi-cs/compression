#ifndef COMPRESSION_BASE_H
#define COMPRESSION_BASE_H

#include <vector>
#include <cstdint>
#include <cstdio>
namespace compression {

class CompressionBase {
public:
    virtual ~CompressionBase() = default;
    
    // Pure virtual functions that derived classes must implement
    virtual std::vector<uint8_t> compress(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed_data) = 0;
    
    // Common utility functions can be added here    
    void print_bytes(const std::vector<uint8_t>& data, size_t bytes_per_row) {
        for (size_t i = 0; i < data.size(); i++) {
            // Print byte in hexadecimal format with leading zeros
            printf("%02X ", data[data.size() - i - 1]);
            
            // Add newline after every bytes_per_row bytes
            if ((i + 1) % bytes_per_row == 0 || i == data.size() - 1) {
                printf("\n");
            }
        }
    }
protected:
    CompressionBase() = default;
};

} // namespace compression

#endif // COMPRESSION_BASE_H 