/*
#include <iostream>
#include <fstream>
using namespace std;

class TreeNode {
public:
    int key;
    streampos filePosition;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int k, streampos pos) : key(k), filePosition(pos), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    TreeNode* root;

    void insert(TreeNode*& node, int key, streampos pos) {
        if (node == nullptr) {
            node = new TreeNode(key, pos);
        }
        else if (key < node->key) {
            insert(node->left, key, pos);
        }
        else if (key > node->key) {
            insert(node->right, key, pos);
        }
    }

    TreeNode* search(TreeNode* node, int key) {
        if (node == nullptr || node->key == key) {
            return node;
        }

        if (key < node->key) {
            return search(node->left, key);
        }

        return search(node->right, key);
    }

    TreeNode* findMin(TreeNode* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    TreeNode* remove(TreeNode*& node, int key) {
        if (node == nullptr) {
            return node;
        }

        if (key < node->key) {
            node->left = remove(node->left, key);
        }
        else if (key > node->key) {
            node->right = remove(node->right, key);
        }
        else {
            if (node->left == nullptr) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            }

            TreeNode* temp = findMin(node->right);
            node->key = temp->key;
            node->right = remove(node->right, temp->key);
        }

        return node;
    }

    void display(TreeNode* node) {
        if (node != nullptr) {
            display(node->left);
            cout << "Key: " << node->key << ", Position: " << node->filePosition;
            if (node->left != nullptr) {
                cout << ", Left: " << node->left->key;
            }
            if (node->right != nullptr) {
                cout << ", Right: " << node->right->key;
            }
            cout << endl;
            display(node->right);
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}

    void insert(int key, streampos pos) {
        insert(root, key, pos);
    }

    TreeNode* search(int key) {
        return search(root, key);
    }

    void remove(int key) {
        root = remove(root, key);
    }

    void display() {
        display(root);
    }
};

class FileManager {
public:
    static void createBinaryFile(const char* textFileName, const char* binaryFileName) {
        ifstream inputFile(textFileName);
        ofstream outputFile(binaryFileName, ios::binary);

        int departurePoint, flightNumber;
        string arrivalDate;

        while (inputFile >> departurePoint >> flightNumber >> arrivalDate) {
            outputFile.write(reinterpret_cast<const char*>(&flightNumber), sizeof(flightNumber));
            streampos pos = outputFile.tellp();
            outputFile.write(reinterpret_cast<const char*>(&departurePoint), sizeof(departurePoint));
            outputFile.write(reinterpret_cast<const char*>(&arrivalDate[0]), arrivalDate.size() + 1);
        }

        inputFile.close();
        outputFile.close();
    }

    static streampos searchRecord(const char* binaryFileName, int key) {
        ifstream file(binaryFileName, ios::binary);

        int currentKey;
        streampos currentPosition;

        while (file.read(reinterpret_cast<char*>(&currentKey), sizeof(currentKey))) {
            file.read(reinterpret_cast<char*>(&currentPosition), sizeof(currentPosition));
            if (currentKey == key) {
                file.close();
                return currentPosition;
            }
            file.ignore(numeric_limits<streamsize>::max(), '\0');
        }

        file.close();
        return -1;
    }
};

int main() {
    const char* textFileName = "C://Users//hyper//Desktop//ВУЗ//СИАОД//Практическая работа 5//data.txt";
    const char* binaryFileName = "C://Users//hyper//Desktop//ВУЗ//СИАОД//Практическая работа 5//data.bin";

    FileManager::createBinaryFile(textFileName, binaryFileName);

    BinarySearchTree bst;

    ifstream binaryFile(binaryFileName, ios::binary);

    int flightNumber;
    streampos currentPosition = 0;

    while (binaryFile.read(reinterpret_cast<char*>(&flightNumber), sizeof(flightNumber))) {
        if (flightNumber <= 10000) {
            bst.insert(flightNumber, currentPosition);
        }
            binaryFile.ignore(numeric_limits<streamsize>::max(), '\0');
            currentPosition = binaryFile.tellg();
    }

    binaryFile.close();

    int searchKey = 101;
    TreeNode* result = bst.search(searchKey);

    if (result != nullptr) {
        cout << "Record found - Key: " << result->key << ", Position: " << result->filePosition << endl;
    }
    else {
        cout << "Record not found for key: " << searchKey << endl;
    }

    bst.display();

    return 0;
}
*/