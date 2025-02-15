// huffman compression algorithm in cpp
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

struct HuffmanNode {
    char data;
    unsigned frequency;
    HuffmanNode *left, *right;
    
    HuffmanNode(char data, unsigned freq) : data(data), frequency(freq), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

class HuffmanCompression {
private:
    HuffmanNode* root = nullptr;
    std::unordered_map<char, std::string> huffmanCodes;
    void generateCodes(HuffmanNode* root, std::string code);
    
public:
    std::string compress(const std::string& input);
    std::string decompress(const std::string& compressed);
    HuffmanNode* buildTree(const std::string& input);
    void printout();
    void removeTree();
};
