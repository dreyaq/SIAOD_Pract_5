#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <chrono>

const int t = 2;

struct BTreeNode {
    std::vector<int> keys;
    std::vector<BTreeNode*> children;
    bool leaf;

    BTreeNode(bool _leaf) : leaf(_leaf) {}
};

class BTree {
private:
    BTreeNode* root;

    void splitChild(BTreeNode* x, int i) {
        BTreeNode* y = x->children[i];
        BTreeNode* z = new BTreeNode(y->leaf);

        x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
        x->children.insert(x->children.begin() + i + 1, z);

        z->keys = std::vector<int>(y->keys.begin() + t, y->keys.end());
        y->keys.resize(t - 1);

        if (!y->leaf) {
            z->children = std::vector<BTreeNode*>(y->children.begin() + t, y->children.end());
            y->children.resize(t);
        }
    }

    void insertNonFull(BTreeNode* x, int key) {
        int i = x->keys.size() - 1;

        if (x->leaf) {
            while (i >= 0 && key < x->keys[i]) {
                x->keys[i + 1] = x->keys[i];
                i--;
            }
            x->keys[i + 1] = key;
        }
        else {
            while (i >= 0 && key < x->keys[i])
                i--;

            i++;
            if (x->children[i]->keys.size() == 2 * t - 1) {
                splitChild(x, i);

                if (key > x->keys[i])
                    i++;
            }

            insertNonFull(x->children[i], key);
        }
    }

public:
    BTree() : root(nullptr) {}

    void insert(int key) {
        if (root == nullptr) {
            root = new BTreeNode(true);
            root->keys.push_back(key);
        }
        else {
            if (root->keys.size() == 2 * t - 1) {
                BTreeNode* s = new BTreeNode(false);
                s->children.push_back(root);
                splitChild(s, 0);
                root = s;
            }
            insertNonFull(root, key);
        }
    }

    bool search(BTreeNode* x, int key) {
        if (x == nullptr)
            return false;

        int i = 0;
        while (i < x->keys.size() && key > x->keys[i])
            i++;

        if (i < x->keys.size() && key == x->keys[i])
            return true;
        else if (x->leaf)
            return false;
        else
            return search(x->children[i], key);
    }

    bool search(int key) {
        return search(root, key);
    }
};

class HashTable {
private:
    std::vector<std::list<int>> table;
    size_t size;

    size_t hashFunction(int key) {
        return key % size;
    }

public:
    HashTable(size_t size) : size(size), table(size) {}

    void insert(int key) {
        size_t index = hashFunction(key);
        table[index].push_back(key);
    }

    bool search(int key) {
        size_t index = hashFunction(key);
        for (int element : table[index]) {
            if (element == key)
                return true;
        }
        return false;
    }
};

struct Node {
    int key;
    Node* left;
    Node* right;

    Node(int val) : key(val), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root;

    Node* insert(Node* root, int key) {
        if (root == nullptr) return new Node(key);

        if (key < root->key)
            root->left = insert(root->left, key);
        else if (key > root->key)
            root->right = insert(root->right, key);

        return root;
    }

    bool search(Node* root, int key) {
        if (root == nullptr) return false;
        if (root->key == key) return true;

        if (key < root->key)
            return search(root->left, key);
        else
            return search(root->right, key);
    }

public:
    BST() : root(nullptr) {}

    void insert(int key) {
        root = insert(root, key);
    }

    bool search(int key) {
        return search(root, key);
    }
};

void testStructure(int n, int searchKey) {
    BST bst;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
        bst.insert(i);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "BST Insert Time: " << duration.count() << " microseconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    bool foundBST = bst.search(searchKey - 1);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "BST Search Time: " << duration.count() << " microseconds. Found: " << searchKey - 1 << std::endl;

    HashTable hashtable(1000);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
        hashtable.insert(i);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "HashTable Insert Time: " << duration.count() << " microseconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    bool foundHashTable = hashtable.search(searchKey - 1);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "HashTable Search Time: " << duration.count() << " microseconds. Found: " << searchKey-1 << std::endl;
}

int main() {
    int n = 100; 
    int searchKey = n / 2 - 1;

    testStructure(n, searchKey);

    std::set<int> smallBTree;
    auto startIns = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        smallBTree.insert(i);
    }
    auto endIns = std::chrono::high_resolution_clock::now();
    std::cout << "B-Tree Insert Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(endIns - startIns)).count() << " microseconds"  <<std::endl;

    // Ищем случайное число в B-дереве и замеряем время выполнения
    auto startSmall = std::chrono::high_resolution_clock::now();
    auto resultSmall = smallBTree.find(searchKey-1);
    auto endSmall = std::chrono::high_resolution_clock::now();

    auto durationSmall = std::chrono::duration_cast<std::chrono::microseconds>(endSmall - startSmall);
    std::cout << "B-Tree Search Time: " << durationSmall.count() << " microseconds. Found: " << *resultSmall << std::endl;
    return 0;
}
