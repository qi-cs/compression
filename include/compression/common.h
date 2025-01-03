#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <cstdint>


template<typename T>
int64_t getValue(const std::vector<uint8_t>& data, size_t offset, size_t i) {
    T temp = 0;
    std::copy(data.begin() + offset + i,
             data.begin() + offset + i + sizeof(T),
             reinterpret_cast<uint8_t*>(&temp));
    return static_cast<int64_t>(temp);
}

#endif // COMMON_H