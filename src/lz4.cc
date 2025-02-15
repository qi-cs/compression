// src/lz4.cc
#include "compression/lz4.h"
#include <cstring>

std::vector<char> LZ4Compressor::compress(const std::vector<char>& input) {
    size_t inputSize = input.size();
    std::vector<char> output;

    bool init = true;

    size_t globalLastMatchPosition = 0;

    for (size_t i = 0; i < inputSize;) {
        bool isMatch = false;
        // get the hash value
        int hashValue = hashFunction(input.data(), i, inputSize);
        // get the last position of the match
        int matchLastPosition = hashTable[hashValue];
        printf("i: %ld, hashValue: %d, matchLastPosition: %d\n", i, hashValue, matchLastPosition);

        if (matchLastPosition != -1) {
            // find the match length
            int matchLength = findMatchLength(input.data(), inputSize, i, matchLastPosition);
            int matchOffset = i - matchLastPosition;
            
            if (matchLength >= LZ4_MIN_MATCH) {
                
                // create a block
                LZ4Block block;

                // compute literal length
                int literalLength;
                if (init == true) {
                    literalLength = i;
                    init = false;
                } else {
                    literalLength = i - matchLastPosition - matchLength;
                }
                printf("literalLength: %d, matchLength: %d\n", literalLength, matchLength);
                // encode the token
                block.token = MIN(literalLength, 15U) | MIN(matchLength - 4, 15U) << 4;

                if (literalLength > 15U) {
                    int tmpLiteralLength = literalLength - 15U;
                    while (tmpLiteralLength > 0) {
                        block.literalLengths.push_back(tmpLiteralLength & 0xFF);
                        tmpLiteralLength = tmpLiteralLength -255;
                    }
                }

                if (matchLength > 19U) {
                    // at least match 4 bytes
                    int tmpMatchLength = matchLength - 19U;
                    while (tmpMatchLength > 0) {
                        block.matchLengths.push_back(tmpMatchLength & 0xFF);
                        tmpMatchLength = tmpMatchLength -255;
                    }
                }

                for (int j = i - literalLength; j < i; ++j) {
                    block.literals.push_back(input[j]);
                }

                block.matchOffset = matchOffset;

                block.print();

                block.encode(output);

                // update the hash table
                hashTable[hashValue] = i;

                i += matchLength;

                globalLastMatchPosition = i;
            }
        }

        if (!isMatch) {
            int hashValue = hashFunction(input.data(), i, inputSize);
            if (hashTable[hashValue] == -1) {
                hashTable[hashValue] = i;
            }
            
            printf("i: %ld does not match.\n", i);

            ++i;
        }
    }

    /*
    * even here we take care of the last block
    * if the last block is not a match, since token contains match length,
    * which start from 4, the last block dont know how to handle it.compress
    * since in real case, might zero match in last block.
    * TODO: need to handle this case.
    */
    if (globalLastMatchPosition < inputSize) {
        // add the last block
        LZ4Block block;
        int literalLength = inputSize - globalLastMatchPosition;
        printf("last block literalLength: %d\n", literalLength);
        block.token = literalLength & 0xF;
        if (literalLength > 15U) {
            int tmpLiteralLength = literalLength - 15U;
            while (tmpLiteralLength > 0) {
                block.literalLengths.push_back(tmpLiteralLength & 0xFF);
                tmpLiteralLength = tmpLiteralLength -255;
            }
        }
        for (int j = globalLastMatchPosition; j < inputSize; ++j) {
            block.literals.push_back(input[j]);
        }
        block.matchOffset = 0;
        block.encode(output);
    }

    return output;
}

std::vector<char> LZ4Compressor::decompress(const std::vector<char>& input) {
    printf("start decompress\n");
    std::vector<char> output;

    // implement the decompression
    for (int i = 0; i < input.size();) {
        printf("#### new token i: %d output size: %ld\n", i, output.size());
        // get the token
        int token = input[i];
        // get the literal length
        int literalLength = token & 0xF;
        // get the match length
        int matchLength = ((token >> 4) & 0xF) + 4;
        printf("literalLength: %d, matchLength: %d\n", literalLength, matchLength);
        // literal is 15, get following bytes until the byte is less than 255
        if (literalLength == 15) {
            int tmpLiteralLength = 0;
            i++;
            while (input[i] == 255) {
                tmpLiteralLength += input[i];
                i++;
            }
            tmpLiteralLength += input[i];
            literalLength += tmpLiteralLength;
        }

        i++;
        // copy the literals
        int endLiteralIndex = i + literalLength;
        for (; i < endLiteralIndex; ++i) {
            printf("literals i: %d, input[i]: %d\n", i, input[i]);
            output.push_back(input[i]);
        }

        if (matchLength != 0) {
            int matchOffset = input[i++] | (input[i++] << 8);
            // match is 19, get following bytes until the byte is less than 255
            if (matchLength == 19) {
                int tmpMatchLength = 0;
                i++;
                while (input[i] == 255) {
                    tmpMatchLength += input[i];
                    i++;
                }
                tmpMatchLength += input[i];
                matchLength += tmpMatchLength;
            }
            // copy the match characters
            int startMatchIndex = output.size() - matchOffset;
            printf("startMatchIndex: %d match offset: %d\n", startMatchIndex, matchOffset);
            for (int j = 0; j < matchLength; ++j) {
                output.push_back(output[startMatchIndex + j]);
            }
        }
    }
    return output;
}


