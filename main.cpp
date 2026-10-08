#include <iostream>
#include <fstream>
using namespace std;

struct Node {
    char ch;
    int frequency;

    Node* left;
    Node* right;
};

int main() {

    ifstream file("input.txt");

    if (!file) {
        cout << "File could not be opened.\n";
        return 1;
    }

    int frequency[256] = {0};
    char ch;

    while (file.get(ch)) {
        frequency[(unsigned char)ch]++;
    }

    file.close();

    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            cout << (char)i << " = " << frequency[i] << endl;
        }
    }

    return 0;
}