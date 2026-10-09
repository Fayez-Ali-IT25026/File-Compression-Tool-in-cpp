
#include <iostream>
#include <fstream>
#include <queue>
using namespace std;

struct Node {
    char ch;
    int frequency;
    Node* left;
    Node* right;
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency;
    }
};

Node* buildTree(int frequency[]) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            Node* node = new Node{
                (char)i, frequency[i], nullptr, nullptr
            };
            pq.push(node);
        }
    }

    if (pq.empty()) return nullptr;

    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();

        Node* right = pq.top();
        pq.pop();

        Node* parent = new Node{
            '\0',
            left->frequency + right->frequency,
            left,
            right
        };

        pq.push(parent);
    }

    return pq.top();
}

void generateCodes(Node* root, string code, string codes[]) {
    if (root == nullptr) return;

    if (root->left == nullptr && root->right == nullptr) {
        codes[(unsigned char)root->ch] = code;
        return;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

void compressFile() {
    ifstream input("input.txt", ios::binary);

    if (!input) {
        cout << "Cannot open input.txt\n";
        return;
    }

    int frequency[256] = {0};
    char ch;

    while (input.get(ch)) {
        frequency[(unsigned char)ch]++;
    }
    input.close();

    Node* root = buildTree(frequency);

    if (root == nullptr) {
        cout << "Input file is empty.\n";
        return;
    }

    string codes[256];
    generateCodes(root, "", codes);

    // A single-character file needs a non-empty code.
    if (root->left == nullptr && root->right == nullptr) {
        codes[(unsigned char)root->ch] = "0";
    }

    ifstream source("input.txt", ios::binary);
    string encodedData;

    while (source.get(ch)) {
        encodedData += codes[(unsigned char)ch];
    }
    source.close();

    ofstream output("compressed.huff", ios::binary);

    if (!output) {
        cout << "Cannot create compressed.huff\n";
        return;
    }

    output.write((char*)frequency, sizeof(frequency));

    unsigned long long bitCount = encodedData.size();
    output.write((char*)&bitCount, sizeof(bitCount));

    unsigned char byte = 0;
    int bitIndex = 0;

    for (char bit : encodedData) {
        if (bit == '1') {
            byte |= (1 << (7 - bitIndex));
        }

        bitIndex++;

        if (bitIndex == 8) {
            output.put(byte);
            byte = 0;
            bitIndex = 0;
        }
    }

    if (bitIndex > 0) {
        output.put(byte);
    }

    output.close();
    cout << "Compressed to compressed.huff\n";
}

void decompressFile() {
    ifstream input("compressed.huff", ios::binary);

    if (!input) {
        cout << "Cannot open compressed.huff\n";
        return;
    }

    int frequency[256] = {0};
    input.read((char*)frequency, sizeof(frequency));

    if (!input) {
        cout << "Invalid compressed file.\n";
        return;
    }

    unsigned long long bitCount;
    input.read((char*)&bitCount, sizeof(bitCount));

    if (!input || bitCount > 100000000ULL) {
        cout << "Invalid compressed file or file too large.\n";
        return;
    }

    Node* root = buildTree(frequency);

    if (root == nullptr) {
        cout << "Invalid frequency table.\n";
        return;
    }

    ofstream output("decompressed.txt", ios::binary);

    if (!output) {
        cout << "Cannot create decompressed.txt\n";
        return;
    }

    // Special case: original file contained one unique character.
    if (root->left == nullptr && root->right == nullptr) {
        for (int i = 0; i < frequency[(unsigned char)root->ch]; i++) {
            output.put(root->ch);
        }

        cout << "Decompressed to decompressed.txt\n";
        return;
    }

    Node* current = root;
    unsigned long long bitsRead = 0;
    char byte;

    while (bitsRead < bitCount && input.get(byte)) {
        unsigned char value = (unsigned char)byte;

        for (int i = 7; i >= 0 && bitsRead < bitCount; i--) {
            int bit = (value >> i) & 1;

            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }

            if (current == nullptr) {
                cout << "Invalid compressed data.\n";
                return;
            }

            if (current->left == nullptr && current->right == nullptr) {
                output.put(current->ch);
                current = root;
            }

            bitsRead++;
        }
    }

    output.close();

    if (bitsRead != bitCount || current != root) {
        cout << "Warning: compressed data may be incomplete.\n";
        return;
    }

    cout << "Decompressed to decompressed.txt\n";
}

int main() {
    int choice;

    cout << "1. Compress\n";
    cout << "2. Decompress\n";
    cout << "3. Exit\n";
    cout << "Choice: ";
    cin >> choice;

    if (choice == 1) {
        compressFile();
    } else if (choice == 2) {
        decompressFile();
    } else if (choice == 3) {
        cout << "Goodbye!\n";
    } else {
        cout << "Invalid choice.\n";
    }

    return 0;
}