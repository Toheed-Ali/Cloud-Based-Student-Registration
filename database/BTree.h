#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

#define ORDER 5  // B-Tree order (max 4 keys per node)

template<typename K, typename V>
class BTreeNode {
public:
    K keys[ORDER - 1];           // Maximum ORDER-1 keys
    V values[ORDER - 1];         // Corresponding values
    BTreeNode* children[ORDER];  // Child pointers
    int numKeys;                 // Current number of keys
    bool isLeaf;                 // Is leaf node?
    
    BTreeNode(bool leaf = true);
    ~BTreeNode();
    
    // Search for a key in this node
    V* search(const K& key);
    
    // Insert a key-value pair (assumes node is not full)
    void insertNonFull(const K& key, const V& value);
    
    // Split child at index i
    void splitChild(int i, BTreeNode* child);
    
    // Find index of first key >= given key
    int findKey(const K& key);
    
    // Remove key from this node
    void remove(const K& key);
    
    // Get predecessor key from subtree
    K getPredecessor(int idx);
    
    // Get successor key from subtree
    K getSuccessor(int idx);
    
    // Borrow from previous sibling
    void borrowFromPrev(int idx);
    
    // Borrow from next sibling
    void borrowFromNext(int idx);
    
    // Merge with sibling
    void merge(int idx);
    
    // Fill child at idx if it has fewer than ORDER/2 keys
    void fill(int idx);
    
    // Serialize node to file
    void serialize(ofstream& out);
    
    // Deserialize node from file
    void deserialize(ifstream& in);
    
    template<typename K2, typename V2>
    friend class BTree;
};

template<typename K, typename V>
class BTree {
private:
    BTreeNode<K, V>* root;
    
    void destroyTree(BTreeNode<K, V>* node);
    void getAllPairs(BTreeNode<K, V>* node, vector<pair<K, V>>& pairs);
    
public:
    BTree();
    ~BTree();
    
    // Search for a key
    V* search(const K& key);
    
    // Insert a key-value pair
    void insert(const K& key, const V& value);
    
    // Update existing key's value
    bool update(const K& key, const V& value);
    
    // Remove a key
    void remove(const K& key);
    
    // Get all key-value pairs (for iteration)
    vector<pair<K, V>> getAllPairs();
    
    // Check if tree is empty
    bool isEmpty() const { return root == nullptr || root->numKeys == 0; }
    
    // Save tree to file
    bool saveToFile(const string& filename);
    
    // Load tree from file
    bool loadFromFile(const string& filename);
    
    // Clear all data
    void clear();
};

// ==================== BTreeNode Implementation ====================

template<typename K, typename V>
BTreeNode<K, V>::BTreeNode(bool leaf) : numKeys(0), isLeaf(leaf) {
    for (int i = 0; i < ORDER; i++) {
        children[i] = nullptr;
    }
}

template<typename K, typename V>
BTreeNode<K, V>::~BTreeNode() {
    // Children are deleted by BTree destructor
}

template<typename K, typename V>
V* BTreeNode<K, V>::search(const K& key) {
    int i = 0;
    while (i < numKeys && key > keys[i]) {
        i++;
    }
    
    if (i < numKeys && key == keys[i]) {
        return &values[i];
    }
    
    if (isLeaf) {
        return nullptr;
    }
    
    return children[i]->search(key);
}

template<typename K, typename V>
int BTreeNode<K, V>::findKey(const K& key) {
    int idx = 0;
    while (idx < numKeys && keys[idx] < key) {
        idx++;
    }
    return idx;
}

template<typename K, typename V>
void BTreeNode<K, V>::insertNonFull(const K& key, const V& value) {
    int i = numKeys - 1;
    
    if (isLeaf) {
        // Find position and shift keys
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            values[i + 1] = values[i];
            i--;
        }
        
        keys[i + 1] = key;
        values[i + 1] = value;
        numKeys++;
    } else {
        // Find child to insert
        while (i >= 0 && keys[i] > key) {
            i--;
        }
        i++;
        
        // Check if child is full
        if (children[i]->numKeys == ORDER - 1) {
            splitChild(i, children[i]);
            
            if (keys[i] < key) {
                i++;
            }
        }
        children[i]->insertNonFull(key, value);
    }
}

template<typename K, typename V>
void BTreeNode<K, V>::splitChild(int i, BTreeNode* child) {
    BTreeNode* newNode = new BTreeNode<K, V>(child->isLeaf);
    newNode->numKeys = ORDER / 2;
    
    // Copy second half of keys to new node
    for (int j = 0; j < ORDER / 2; j++) {
        newNode->keys[j] = child->keys[j + ORDER / 2];
        newNode->values[j] = child->values[j + ORDER / 2];
    }
    
    // Copy children if not leaf
    if (!child->isLeaf) {
        for (int j = 0; j <= ORDER / 2; j++) {
            newNode->children[j] = child->children[j + ORDER / 2];
        }
    }
    
    child->numKeys = ORDER / 2 - 1;
    
    // Shift children of this node
    for (int j = numKeys; j >= i + 1; j--) {
        children[j + 1] = children[j];
    }
    children[i + 1] = newNode;
    
    // Shift keys of this node
    for (int j = numKeys - 1; j >= i; j--) {
        keys[j + 1] = keys[j];
        values[j + 1] = values[j];
    }
    
    // Copy middle key up
    keys[i] = child->keys[ORDER / 2 - 1];
    values[i] = child->values[ORDER / 2 - 1];
    numKeys++;
}

template<typename K, typename V>
void BTreeNode<K, V>::remove(const K& key) {
    int idx = findKey(key);
    
    if (idx < numKeys && keys[idx] == key) {
        if (isLeaf) {
            // Remove from leaf
            for (int i = idx + 1; i < numKeys; i++) {
                keys[i - 1] = keys[i];
                values[i - 1] = values[i];
            }
            numKeys--;
        } else {
            // Remove from internal node
            if (children[idx]->numKeys >= ORDER / 2) {
                K pred = getPredecessor(idx);
                keys[idx] = pred;
                children[idx]->remove(pred);
            } else if (children[idx + 1]->numKeys >= ORDER / 2) {
                K succ = getSuccessor(idx);
                keys[idx] = succ;
                children[idx + 1]->remove(succ);
            } else {
                merge(idx);
                children[idx]->remove(key);
            }
        }
    } else {
        if (isLeaf) {
            return;  // Key not found
        }
        
        bool isInSubtree = (idx == numKeys);
        
        if (children[idx]->numKeys < ORDER / 2) {
            fill(idx);
        }
        
        if (isInSubtree && idx > numKeys) {
            children[idx - 1]->remove(key);
        } else {
            children[idx]->remove(key);
        }
    }
}

template<typename K, typename V>
K BTreeNode<K, V>::getPredecessor(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->isLeaf) {
        cur = cur->children[cur->numKeys];
    }
    return cur->keys[cur->numKeys - 1];
}

template<typename K, typename V>
K BTreeNode<K, V>::getSuccessor(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->isLeaf) {
        cur = cur->children[0];
    }
    return cur->keys[0];
}

template<typename K, typename V>
void BTreeNode<K, V>::fill(int idx) {
    if (idx != 0 && children[idx - 1]->numKeys >= ORDER / 2) {
        borrowFromPrev(idx);
    } else if (idx != numKeys && children[idx + 1]->numKeys >= ORDER / 2) {
        borrowFromNext(idx);
    } else {
        if (idx != numKeys) {
            merge(idx);
        } else {
            merge(idx - 1);
        }
    }
}

template<typename K, typename V>
void BTreeNode<K, V>::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];
    
    // Move keys in child
    for (int i = child->numKeys - 1; i >= 0; i--) {
        child->keys[i + 1] = child->keys[i];
        child->values[i + 1] = child->values[i];
    }
    
    // Move children
    if (!child->isLeaf) {
        for (int i = child->numKeys; i >= 0; i--) {
            child->children[i + 1] = child->children[i];
        }
    }
    
    child->keys[0] = keys[idx - 1];
    child->values[0] = values[idx - 1];
    
    if (!child->isLeaf) {
        child->children[0] = sibling->children[sibling->numKeys];
    }
    
    keys[idx - 1] = sibling->keys[sibling->numKeys - 1];
    values[idx - 1] = sibling->values[sibling->numKeys - 1];
    
    child->numKeys++;
    sibling->numKeys--;
}

template<typename K, typename V>
void BTreeNode<K, V>::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];
    
    child->keys[child->numKeys] = keys[idx];
    child->values[child->numKeys] = values[idx];
    
    if (!child->isLeaf) {
        child->children[child->numKeys + 1] = sibling->children[0];
    }
    
    keys[idx] = sibling->keys[0];
    values[idx] = sibling->values[0];
    
    for (int i = 1; i < sibling->numKeys; i++) {
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->values[i - 1] = sibling->values[i];
    }
    
    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->numKeys; i++) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }
    
    child->numKeys++;
    sibling->numKeys--;
}

template<typename K, typename V>
void BTreeNode<K, V>::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];
    
    child->keys[child->numKeys] = keys[idx];
    child->values[child->numKeys] = values[idx];
    
    for (int i = 0; i < sibling->numKeys; i++) {
        child->keys[i + child->numKeys + 1] = sibling->keys[i];
        child->values[i + child->numKeys + 1] = sibling->values[i];
    }
    
    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->numKeys; i++) {
            child->children[i + child->numKeys + 1] = sibling->children[i];
        }
    }
    
    for (int i = idx + 1; i < numKeys; i++) {
        keys[i - 1] = keys[i];
        values[i - 1] = values[i];
    }
    
    for (int i = idx + 2; i <= numKeys; i++) {
        children[i - 1] = children[i];
    }
    
    child->numKeys += sibling->numKeys + 1;
    numKeys--;
    
    delete sibling;
}

// ==================== BTree Implementation ====================

template<typename K, typename V>
BTree<K, V>::BTree() : root(nullptr) {}

template<typename K, typename V>
BTree<K, V>::~BTree() {
    destroyTree(root);
}

template<typename K, typename V>
void BTree<K, V>::destroyTree(BTreeNode<K, V>* node) {
    if (node == nullptr) return;
    
    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++) {
            destroyTree(node->children[i]);
        }
    }
    delete node;
}

template<typename K, typename V>
V* BTree<K, V>::search(const K& key) {
    if (root == nullptr) return nullptr;
    return root->search(key);
}

template<typename K, typename V>
void BTree<K, V>::insert(const K& key, const V& value) {
    if (root == nullptr) {
        root = new BTreeNode<K, V>(true);
        root->keys[0] = key;
        root->values[0] = value;
        root->numKeys = 1;
        return;
    }
    
    // Check if key already exists (update instead)
    if (update(key, value)) {
        return;
    }
    
    if (root->numKeys == ORDER - 1) {
        BTreeNode<K, V>* newRoot = new BTreeNode<K, V>(false);
        newRoot->children[0] = root;
        newRoot->splitChild(0, root);
        
        int i = 0;
        if (newRoot->keys[0] < key) {
            i++;
        }
        newRoot->children[i]->insertNonFull(key, value);
        
        root = newRoot;
    } else {
        root->insertNonFull(key, value);
    }
}

template<typename K, typename V>
bool BTree<K, V>::update(const K& key, const V& value) {
    V* found = search(key);
    if (found != nullptr) {
        *found = value;
        return true;
    }
    return false;
}

template<typename K, typename V>
void BTree<K, V>::remove(const K& key) {
    if (root == nullptr) return;
    
    root->remove(key);
    
    if (root->numKeys == 0) {
        BTreeNode<K, V>* oldRoot = root;
        if (root->isLeaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete oldRoot;
    }
}

template<typename K, typename V>
void BTree<K, V>::getAllPairs(BTreeNode<K, V>* node, vector<pair<K, V>>& pairs) {
    if (node == nullptr) return;
    
    int i;
    for (i = 0; i < node->numKeys; i++) {
        if (!node->isLeaf) {
            getAllPairs(node->children[i], pairs);
        }
        pairs.push_back({node->keys[i], node->values[i]});
    }
    
    if (!node->isLeaf) {
        getAllPairs(node->children[i], pairs);
    }
}

template<typename K, typename V>
vector<pair<K, V>> BTree<K, V>::getAllPairs() {
    vector<pair<K, V>> pairs;
    getAllPairs(root, pairs);
    return pairs;
}

template<typename K, typename V>
void BTree<K, V>::clear() {
    destroyTree(root);
    root = nullptr;
}

template<typename K, typename V>
bool BTree<K, V>::saveToFile(const string& filename) {
    // Note: Simplified serialization - in production, implement proper node serialization
    ofstream out(filename, ios::binary);
    if (!out.is_open()) return false;
    
    vector<pair<K, V>> pairs = getAllPairs();
    size_t count = pairs.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    for (const auto& p : pairs) {
        out.write(reinterpret_cast<const char*>(&p.first), sizeof(K));
        out.write(reinterpret_cast<const char*>(&p.second), sizeof(V));
    }
    
    out.close();
    return true;
}

template<typename K, typename V>
bool BTree<K, V>::loadFromFile(const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) return false;
    
    clear();
    
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (size_t i = 0; i < count; i++) {
        K key;
        V value;
        in.read(reinterpret_cast<char*>(&key), sizeof(K));
        in.read(reinterpret_cast<char*>(&value), sizeof(V));
        insert(key, value);
    }
    
    in.close();
    return true;
}

#endif // BTREE_H
