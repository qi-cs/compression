// include/lz4.h
#ifndef LZ4_H
#define LZ4_H

#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include <algorithm>

#define MIN(x,y) (std::min(static_cast<size_t>(x), static_cast<size_t>(y)))

const int MAGIC_NUMBER = 0x9E3779B9;
const int HASH_TABLE_SIZE = 65536;
constexpr int LZ4_MIN_MATCH = 4;
constexpr int LZ4_MAX_DISTANCE = 65535;
const int WINDOW_SIZE = 64 * 1024; // 64 KB


class LZ4Compressor {
private:

    // hash table initialized with -1
    int hashTable[HASH_TABLE_SIZE] = {-1}; // Stores positions of sequences
public:
    LZ4Compressor() {
        // initialize the hash table with -1
        std::fill(hashTable, hashTable + HASH_TABLE_SIZE, -1);
    }
public:
    struct LZ4Block {
        // 1 bytes
        uint8_t token;
        // 0 - n bytes
        std::vector<uint8_t> literalLengths;
        // 0 - L bytes
        std::vector<uint8_t> literals;
        // 2 bytes
        uint16_t matchOffset;
        // 0 - n bytes
        std::vector<uint8_t> matchLengths;

        // print the block
        void print() {
            std::cout << "Token: " << static_cast<int>(token) << std::endl;
            int literalLength = token & 0xF;
            for (auto& literal : literalLengths) {
                literalLength += literal;
            }
            std::cout << "Literal Length: " << literalLength << std::endl;
            std::cout << "Literals: ";
            for (auto& literal : literals) {
                // print literal as char
                std::cout << static_cast<char>(literal) << " ";
            }
            std::cout << std::endl;
            std::cout << "Match Offset: " << matchOffset << std::endl;
            int matchLength = ((token >> 4) & 0xF) + 4;
            for (auto& match : matchLengths) {
                matchLength += match;
            }
            std::cout << "Match Length: " << matchLength << std::endl;
        }

        // encode the block
        void encode(std::vector<char>& output) {
            // count increased size of output
            int originalSize = output.size();
            // encode the token
            output.push_back(token);
            // encode the literal lengths
            for (auto& literal : literalLengths) {
                output.push_back(literal);
            }   
            // encode the literals
            for (auto& literal : literals) {
                output.push_back(literal);
            }
            // encode the match offset
            output.push_back(matchOffset & 0xFF);
            output.push_back((matchOffset >> 8) & 0xFF);
            // encode the match lengths
            for (auto& match : matchLengths) {
                output.push_back(match);
            }

        }   
    };

    // hash function
    int hashFunction(const char* data, size_t index, size_t length) {
        int64_t hash_value = 0;
        for (int i = 0; i < MIN(length - index, 4); ++i) {
            hash_value = (hash_value << 8) + data[index + i];
        }

        hash_value = hash_value ^ MAGIC_NUMBER;

        int hash_value_positive = hash_value % HASH_TABLE_SIZE;
        // ensure hash_value is positive
        if (hash_value_positive < 0) {
            hash_value_positive = -hash_value_positive;
        }
        return hash_value_positive;
    }

    // find the match length
    int findMatchLength(const char* data, size_t dataSize, size_t currentIndex, size_t candidateIndex) {
        int matchLength = 0;
        while ((currentIndex + matchLength < dataSize && data[currentIndex + matchLength] == data[candidateIndex + matchLength]) && 
        (candidateIndex + matchLength < currentIndex)) {
            matchLength++;
        }
        return matchLength;
    }

    void printOutput(const std::vector<char>& output) {
        for (auto& byte : output) {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }

    std::vector<char> compress(const std::vector<char>& input);
    std::vector<char> decompress(const std::vector<char>& input);
};

#endif // LZ4_H
