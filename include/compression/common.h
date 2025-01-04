#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <list>
#include <string>
#include <optional>

using std::vector;
using std::pair;

template<typename T>
int64_t getValue(const std::vector<uint8_t>& data, size_t offset, size_t i) {
    T temp = 0;
    std::copy(data.begin() + offset + i,
             data.begin() + offset + i + sizeof(T),
             reinterpret_cast<uint8_t*>(&temp));
    return static_cast<int64_t>(temp);
}


class Dictionary {
private:
    // Maximum size of the dictionary
    size_t max_size_;

    // If true, the dictionary is frozen and no new entries can be added
    // no more update of lru
    bool freeze_ = false;

    // Structure to hold the dictionary entry and its metadata
    struct Entry {
        uint32_t key;      // Full 32-bit key
        uint32_t value; // Value associated with the key
    };
    
    // LRU list to track usage (most recent at front)
    std::list<uint32_t> lru_list_;
    
    // Main storage: key -> (value, lru_iterator)
    std::unordered_map<uint32_t, std::pair<uint32_t, std::list<uint32_t>::iterator>> storage_;

    // Update LRU for a given key
    void update_lru(uint32_t key) {
        if (freeze_) {
            return;
        }

        auto it = storage_.find(key);
        if (it != storage_.end()) {
            lru_list_.erase(it->second.second);
            lru_list_.push_front(key);
            it->second.second = lru_list_.begin();
        }
    }

    // Remove least recently used entry
    void remove_lru() {
        if (!lru_list_.empty()) {
            uint32_t last_key = lru_list_.back();
            lru_list_.pop_back();
            storage_.erase(last_key);
        }
    }

public:
    explicit Dictionary(size_t max_size = 1000) : max_size_(max_size) {}

    // Insert or update a key-value pair
    void insert(uint32_t key, const uint32_t& value) {
        // If key exists, update it
        if (storage_.find(key) != storage_.end()) {
            storage_[key].first = value;
            update_lru(key);
            return;
        }

        // If at capacity, remove LRU entry
        if (storage_.size() >= max_size_) {
            remove_lru();
        }

        // Insert new entry
        lru_list_.push_front(key);
        storage_[key] = {value, lru_list_.begin()};
    }

    // Search by exact 32-bit key
    bool find_exact(uint32_t key) {
        auto it = storage_.find(key);
        if (it != storage_.end()) {
            update_lru(key);
            return true;
        }
        return  false;
    }

    // Search by first 24 bits
    bool find_24bit(uint32_t prefix) {
        uint32_t mask = 0xFFFFFF00;
        prefix &= mask;

        for (const auto& [key, value] : storage_) {
            if ((key & mask) == prefix) {
                update_lru(key);
                return true;
            }
        }
        return false;
    }

    // Search by first 16 bits
    bool find_16bit(uint32_t prefix) {
        uint32_t mask = 0xFFFF0000;
        prefix &= mask;

        for (const auto& [key, value] : storage_) {
            if ((key & mask) == prefix) {
                update_lru(key);
                return true;
            }
        }
        return false;
    }

    // Get current size
    size_t size() const {
        return storage_.size();
    }

    // Clear the dictionary
    void clear() {
        storage_.clear();
        lru_list_.clear();
    }

    void setFreeze(bool freeze) {
        freeze_ = freeze;
    }

    // print the dictionary
    void print() const {
        printf("Dictionary size: %zu\n", storage_.size());
        for (const auto& [key, value] : storage_) {
            printf("key: %08X, value: %08X\n", key, value.first);
        }
    }
};

#endif // COMMON_H
