/*
#include <iostream>
#include <vector>

const int MIN_KEYS = 1;  // Минимальное количество ключей в узле
const int MAX_KEYS = 2;  // Максимальное количество ключей в узле

struct Node {
    std::vector<int> keys;          // Вектор ключей
    std::vector<int> addresses;     // Вектор адресов записей в файле
    std::vector<Node*> children;    // Вектор дочерних узлов
    bool isLeaf;                    // Признак листа дерева
};

class BTree {
public:
    BTree();
    ~BTree();

    void insert(int key, int address);
    void remove(int key);
    int search(int key);
    void display();
    Node* getRoot();

private:
    Node* root;

    Node* createNode();
    void destroyTree(Node* node);
    void splitChild(Node* parent, int index);
    void insertNonFull(Node* node, int key, int address);
    void removeKeyFromNode(Node* node, int key);
    void removeFromLeaf(Node* node, int index);
    void borrowFromPrev(Node* node, int index);
    void borrowFromNext(Node* node, int index);
    void mergeNodes(Node* node, int index);
    int findKeyIndex(Node* node, int key);
    int getPredecessor(Node* node, int index);
    int getSuccessor(Node* node, int index);
    void fillNode(Node* node, int index);
    void removeFromNonLeaf(Node* node, int index);
    void displayHelper(Node* root, int level, int nextLevel);
};

Node* BTree::getRoot()
{
    return root;
}

BTree::BTree() {
    root = nullptr;
}

BTree::~BTree() {
    destroyTree(root);
}

Node* BTree::createNode() {
    Node* newNode = new Node;
    newNode->isLeaf = true;
    return newNode;
}

void BTree::destroyTree(Node* node) {
    if (node != nullptr) {
        for (int i = 0; i < node->children.size(); ++i) {
            destroyTree(node->children[i]);
        }
        delete node;
    }
}

void BTree::splitChild(Node* parent, int index) {
    Node* newChild = createNode();
    Node* oldChild = parent->children[index];

    parent->keys.insert(parent->keys.begin() + index, oldChild->keys[MAX_KEYS - 1]);
    parent->addresses.insert(parent->addresses.begin() + index, oldChild->addresses[MAX_KEYS - 1]);

    newChild->keys.assign(oldChild->keys.begin() + MIN_KEYS, oldChild->keys.end());
    newChild->addresses.assign(oldChild->addresses.begin() + MIN_KEYS, oldChild->addresses.end());

    oldChild->keys.resize(MIN_KEYS);
    oldChild->addresses.resize(MIN_KEYS);

    if (!oldChild->isLeaf) {
        newChild->isLeaf = false;
        newChild->children.assign(oldChild->children.begin() + MIN_KEYS, oldChild->children.end());
        oldChild->children.resize(MIN_KEYS + 1);
    }

    parent->children.insert(parent->children.begin() + index + 1, newChild);
}

void BTree::insert(int key, int address) {
    if (root == nullptr) {
        root = createNode();
        root->keys.push_back(key);
        root->addresses.push_back(address);
    }
    else {
        if (root->keys.size() == MAX_KEYS) {
            Node* newRoot = createNode();
            newRoot->isLeaf = false;
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, key, address);
    }

}

void BTree::insertNonFull(Node* node, int key, int address) {
    int index = node->keys.size() - 1;

    if (node->isLeaf) {
        node->keys.push_back(0);
        node->addresses.push_back(0);

        while (index >= 0 && key < node->keys[index]) {
            node->keys[index + 1] = node->keys[index];
            node->addresses[index + 1] = node->addresses[index];
            --index;
        }

        node->keys[index + 1] = key;
        node->addresses[index + 1] = address;
    }
    else {
        while (index >= 0 && key < node->keys[index]) {
            --index;
        }

        ++index;

        if (node->children[index]->keys.size() == MAX_KEYS) {
            splitChild(node, index);

            if (key > node->keys[index]) {
                ++index;
            }
        }

        insertNonFull(node->children[index], key, address);
    }
}

void BTree::remove(int key) {
    if (root == nullptr) {
        return;
    }

    removeKeyFromNode(root, key);

    if (root->keys.size() == 0) {
        Node* oldRoot = root;
        if (root->isLeaf) {
            root = nullptr;
        }
        else {
            root = root->children[0];
        }
        delete oldRoot;
    }
}

void BTree::removeKeyFromNode(Node* node, int key) {
    int index = findKeyIndex(node, key);

    if (index < node->keys.size() && key == node->keys[index]) {
        if (node->isLeaf) {
            removeFromLeaf(node, index);
        }
        else {
            removeFromNonLeaf(node, index);
        }
    }
    else {
        if (node->isLeaf) {
            std::cout << "Key " << key << " not found in the tree.\n";
            return;
        }

        bool lastChild = (index == node->keys.size());

        if (node->children[index]->keys.size() < MIN_KEYS) {
            fillNode(node, index);
        }

        if (lastChild && index > node->keys.size()) {
            removeKeyFromNode(node->children[index - 1], key);
        }
        else {
            removeKeyFromNode(node->children[index], key);
        }
    }
}

void BTree::removeFromLeaf(Node* node, int index) {
    if (index < node->keys.size()) {
        node->keys.erase(node->keys.begin() + index);
        node->addresses.erase(node->addresses.begin() + index);
    }
}

void BTree::removeFromNonLeaf(Node* node, int index) {
    int key = node->keys[index];

    if (node->children[index]->keys.size() >= MIN_KEYS) {
        int predecessor = getPredecessor(node, index);
        node->keys[index] = predecessor;
        removeKeyFromNode(node->children[index], predecessor);
    }
    else if (node->children[index + 1]->keys.size() >= MIN_KEYS) {
        int successor = getSuccessor(node, index);
        node->keys[index] = successor;
        removeKeyFromNode(node->children[index + 1], successor);
    }
    else {
        mergeNodes(node, index);
        removeKeyFromNode(node->children[index], key);
    }
}

int BTree::findKeyIndex(Node* node, int key) {
    int index = 0;
    while (index < node->keys.size() && key > node->keys[index]) {
        ++index;
    }
    return index;
}

int BTree::getPredecessor(Node* node, int index) {
    Node* current = node->children[index];
    while (!current->isLeaf) {
        current = current->children.back();
    }
    return current->keys.back();
}

int BTree::getSuccessor(Node* node, int index) {
    Node* current = node->children[index + 1];
    while (!current->isLeaf) {
        current = current->children.front();
    }
    return current->keys.front();
}

void BTree::fillNode(Node* node, int index) {
    if (index != 0 && node->children[index - 1]->keys.size() >= MIN_KEYS) {
        borrowFromPrev(node, index);
    }
    else if (index != node->keys.size() && node->children[index + 1]->keys.size() >= MIN_KEYS) {
        borrowFromNext(node, index);
    }
    else {
        if (index != node->keys.size()) {
            mergeNodes(node, index);
        }
        else {
            mergeNodes(node, index - 1);
        }
    }
}

void BTree::borrowFromPrev(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index - 1];

    if (!child->isLeaf && !sibling->children.empty()) {
        child->children.insert(child->children.begin(), sibling->children.back());
        sibling->children.pop_back();
    }

    node->keys[index - 1] = sibling->keys.back();
    node->addresses[index - 1] = sibling->addresses.back();

    sibling->keys.pop_back();
    sibling->addresses.pop_back();
}

void BTree::borrowFromNext(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];

    if (!child->isLeaf && !sibling->children.empty()) {
        child->children.push_back(sibling->children.front());
        sibling->children.erase(sibling->children.begin());
    }

    node->keys[index] = sibling->keys.front();
    node->addresses[index] = sibling->addresses.front();

    sibling->keys.erase(sibling->keys.begin());
    sibling->addresses.erase(sibling->addresses.begin());
}

void BTree::mergeNodes(Node* node, int index) {
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];

    if (!child->isLeaf && !sibling->children.empty()) {
        child->children.insert(child->children.end(), sibling->children.begin(), sibling->children.end());
    }

    child->keys.push_back(node->keys[index]);
    child->addresses.push_back(node->addresses[index]);

    child->keys.insert(child->keys.end(), sibling->keys.begin(), sibling->keys.end());
    child->addresses.insert(child->addresses.end(), sibling->addresses.begin(), sibling->addresses.end());

    if (!child->isLeaf) {
        child->children.insert(child->children.end(), sibling->children.begin(), sibling->children.end());
    }

    node->keys.erase(node->keys.begin() + index);
    node->addresses.erase(node->addresses.begin() + index);
    node->children.erase(node->children.begin() + index + 1);

    delete sibling;
}

int BTree::search(int key) {
    Node* current = root;
    while (current != nullptr) {
        int index = 0;
        while (index < current->keys.size() && key > current->keys[index]) {
            ++index;
        }

        if (index < current->keys.size() && key == current->keys[index]) {
            return current->addresses[index];
        }

        if (current->isLeaf) {
            break;
        }

        current = current->children[index];
    }

    std::cout << "Key " << key << " not found in the tree.\n";
    return -1;
}

#include <queue>

void BTree::display() {
    if (root == nullptr) {
        return;
    }

    std::queue<Node*> nodesQueue;
    nodesQueue.push(root);

    int currentLevel = 0;

    while (!nodesQueue.empty()) {
        int nodesAtCurrentLevel = nodesQueue.size();
        std::cout << "Level " << currentLevel << ": ";

        while (nodesAtCurrentLevel > 0) {
            Node* currentNode = nodesQueue.front();
            nodesQueue.pop();

            for (int i = 0; i < currentNode->keys.size(); ++i) {
                std::cout << currentNode->keys[i] << " ";
            }

            if (!currentNode->isLeaf) {
                for (int i = 0; i < currentNode->children.size(); ++i) {
                    nodesQueue.push(currentNode->children[i]);
                }
            }

            nodesAtCurrentLevel--;

            if (nodesAtCurrentLevel > 0) {
                std::cout << " | ";
            }
        }

        std::cout << "\n";
        currentLevel++;
    }
}

int main() {
    BTree btree;

    // Пример вставки
    btree.insert(10, 100);
    btree.insert(5, 50);
    btree.insert(6, 60);
    btree.insert(30, 120);
    btree.insert(12, 300);
    // Пример вывода дерева
    btree.display();

    // Пример поиска
    int searchResult = btree.search(6);
    if (searchResult != -1) {
        std::cout << "Found: " << searchResult << "\n";
    }

    // Пример удаления
    btree.remove(10);

    // Пример вывода дерева после удаления
    btree.display();

    return 0;
}
*/