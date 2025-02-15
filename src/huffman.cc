#include "compression/huffman.h"
#include <functional>
void HuffmanCompression::generateCodes(HuffmanNode* root, std::string code) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
    }
    generateCodes(root->left, code + "0");
    generateCodes(root->right, code + "1");
}

void HuffmanCompression::printout() {
    for (auto pair : huffmanCodes) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
}

HuffmanNode* HuffmanCompression::buildTree(const std::string& input) {
    std::unordered_map<char, unsigned> frequencies;
    for (char c : input) frequencies[c]++;
    // use priority queue to store nodes with lowest frequency at the top
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> minHeap;

    // the leaf nodes are pushed into the minHeap
    // the nodes are sorted based on their frequency
    for (auto pair : frequencies) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }
    // build the huffman tree
    // the two nodes with the lowest frequency are combined into a single node
    // the combined node is then pushed back into the minHeap
    while (minHeap.size() > 1) {
        HuffmanNode *left = minHeap.top(); minHeap.pop();
        HuffmanNode *right = minHeap.top(); minHeap.pop();
        
        HuffmanNode *parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    // return the root of the huffman tree
    // user can use this root to generate huffman codes
    return minHeap.top();
}

std::string HuffmanCompression::compress(const std::string& input) {
    // build the huffman tree
    root = buildTree(input);
    // generate huffman codes
    generateCodes(root, "");

    printout();

    std::string compressed;
    for (char c : input) {
        compressed += huffmanCodes[c];
    }
    return compressed;
}

std::string HuffmanCompression::decompress(const std::string& compressed) {
    std::string decompressed;
    HuffmanNode* current = root;
    
    for (char bit : compressed) {
        // walk the tree based on the bit value
        if (bit == '0') current = current->left;
        else current = current->right;

        // when we reach a leaf node, append the character to the decompressed string
        if (!current->left && !current->right) {
            decompressed += current->data;
            current = root;
        }
    }

    removeTree();
    return decompressed;
}

void HuffmanCompression::removeTree() {
    // traverse the tree in post-order and delete each node
    std::function<void(HuffmanNode*)> deleteTree = [&](HuffmanNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    };
    deleteTree(root);
    root = nullptr;
}