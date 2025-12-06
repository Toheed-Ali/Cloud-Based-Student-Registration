#ifndef BTREE_H
#define BTREE_H

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

template<typename KeyType, typename ValueType>
class BTree {
private:
    struct Node {
        vector<KeyType> keys;
        vector<ValueType> values;
        vector<Node*> children;
        bool isLeaf;
        int n; // current number of keys

        Node(bool leaf) : isLeaf(leaf), n(0) {}
    };

    Node* root;
    int t; // minimum degree (order = 2*t)

    // Helpers for serialization
    template<typename T>
    void writeItem(ofstream& out, const T& val) {
        out.write((const char*)&val, sizeof(T));
    }
    void writeItem(ofstream& out, const string& val) {
        int len = val.size();
        out.write((const char*)&len, sizeof(int));
        out.write(val.c_str(), len);
    }
    template<typename T>
    void readItem(ifstream& in, T& val) {
        in.read((char*)&val, sizeof(T));
    }
    void readItem(ifstream& in, string& val) {
        int len;
        in.read((char*)&len, sizeof(int));
        char* buf = new char[len + 1];
        in.read(buf, len);
        buf[len] = '\0';
        val = string(buf);
        delete[] buf;
    }

    void splitChild(Node* x, int i, Node* y) {
        Node* z = new Node(y->isLeaf);
        z->n = t - 1;

        for (int j = 0; j < t - 1; j++) {
            z->keys.push_back(y->keys[j + t]);
            z->values.push_back(y->values[j + t]);
        }

        if (!y->isLeaf) {
            for (int j = 0; j < t; j++)
                z->children.push_back(y->children[j + t]);
        }

        y->n = t - 1;
        // Resize y's vectors AFTER moving the median key to x
        // y->keys.resize(t - 1);
        // y->values.resize(t - 1);
        if (!y->isLeaf) y->children.resize(t);

        x->children.insert(x->children.begin() + i + 1, z);
        x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
        x->values.insert(x->values.begin() + i, y->values[t - 1]);
        x->n = x->n + 1;
        
        // Now it's safe to resize
        y->keys.resize(t - 1);
        y->values.resize(t - 1);
    }

    void insertNonFull(Node* x, KeyType k, ValueType v) {
        int i = x->n - 1;

        if (x->isLeaf) {
            x->keys.push_back(k);
            x->values.push_back(v);
            
            while (i >= 0 && k < x->keys[i]) {
                x->keys[i + 1] = x->keys[i];
                x->values[i + 1] = x->values[i];
                i--;
            }
            x->keys[i + 1] = k;
            x->values[i + 1] = v;
            x->n = x->n + 1;
        } else {
            while (i >= 0 && k < x->keys[i])
                i--;
            i++;
            if (x->children[i]->n == 2 * t - 1) {
                splitChild(x, i, x->children[i]);
                if (k > x->keys[i])
                    i++;
            }
            insertNonFull(x->children[i], k, v);
        }
    }

    ValueType* search(Node* x, KeyType k) {
        int i = 0;
        while (i < x->n && k > x->keys[i])
            i++;
        
        if (i < x->n && k == x->keys[i])
            return &(x->values[i]);
        
        if (x->isLeaf)
            return nullptr;
            
        return search(x->children[i], k);
    }

    void traverse(Node* x) {
        int i;
        for (i = 0; i < x->n; i++) {
            if (!x->isLeaf)
                traverse(x->children[i]);
            cout << "Key: " << x->keys[i] << endl;
        }
        if (!x->isLeaf)
            traverse(x->children[i]);
    }

    void collectKeys(Node* x, vector<KeyType>& result) {
        int i;
        for (i = 0; i < x->n; i++) {
            if (!x->isLeaf)
                collectKeys(x->children[i], result);
            result.push_back(x->keys[i]);
        }
        if (!x->isLeaf)
            collectKeys(x->children[i], result);
    }

    void collectValues(Node* x, vector<ValueType>& result) {
        int i;
        for (i = 0; i < x->n; i++) {
            if (!x->isLeaf)
                collectValues(x->children[i], result);
            result.push_back(x->values[i]);
        }
        if (!x->isLeaf)
            collectValues(x->children[i], result);
    }

public:
    BTree(int _t) : t(_t), root(nullptr) {}

    void insert(KeyType k, ValueType v) {
        if (root == nullptr) {
            root = new Node(true);
            root->keys.push_back(k);
            root->values.push_back(v);
            root->n = 1;
        } else {
            if (root->n == 2 * t - 1) {
                Node* s = new Node(false);
                s->children.push_back(root);
                splitChild(s, 0, root);
                int i = 0;
                if (s->keys[0] < k)
                    i++;
                insertNonFull(s->children[i], k, v);
                root = s;
            } else {
                insertNonFull(root, k, v);
            }
        }
    }

    // Removal helpers
    int findKey(Node* x, KeyType k) {
        int idx = 0;
        while (idx < x->n && x->keys[idx] < k)
            ++idx;
        return idx;
    }

    void remove(Node* x, KeyType k) {
        int idx = findKey(x, k);

        if (idx < x->n && x->keys[idx] == k) {
            if (x->isLeaf)
                removeFromLeaf(x, idx);
            else
                removeFromNonLeaf(x, idx);
        } else {
            if (x->isLeaf) {
                // Key not found
                return;
            }

            bool flag = (idx == x->n);
            if (x->children[idx]->n < t)
                fill(x, idx);

            if (flag && idx > x->n)
                remove(x->children[idx - 1], k);
            else
                remove(x->children[idx], k);
        }
    }

    void removeFromLeaf(Node* x, int idx) {
        for (int i = idx + 1; i < x->n; ++i) {
            x->keys[i - 1] = x->keys[i];
            x->values[i - 1] = x->values[i];
        }
        x->n--;
        x->keys.resize(x->n);
        x->values.resize(x->n);
    }

    void removeFromNonLeaf(Node* x, int idx) {
        KeyType k = x->keys[idx];

        if (x->children[idx]->n >= t) {
            Node* cur = x->children[idx];
            while (!cur->isLeaf) cur = cur->children[cur->n];
            KeyType predKey = cur->keys[cur->n - 1];
            ValueType predVal = cur->values[cur->n - 1];
            x->keys[idx] = predKey;
            x->values[idx] = predVal;
            remove(x->children[idx], predKey);
        } else if (x->children[idx + 1]->n >= t) {
            Node* cur = x->children[idx + 1];
            while (!cur->isLeaf) cur = cur->children[0];
            KeyType succKey = cur->keys[0];
            ValueType succVal = cur->values[0];
            x->keys[idx] = succKey;
            x->values[idx] = succVal;
            remove(x->children[idx + 1], succKey);
        } else {
            merge(x, idx);
            remove(x->children[idx], k);
        }
    }

    void fill(Node* x, int idx) {
        if (idx != 0 && x->children[idx - 1]->n >= t)
            borrowFromPrev(x, idx);
        else if (idx != x->n && x->children[idx + 1]->n >= t)
            borrowFromNext(x, idx);
        else {
            if (idx != x->n)
                merge(x, idx);
            else
                merge(x, idx - 1);
        }
    }

    void borrowFromPrev(Node* x, int idx) {
        Node* child = x->children[idx];
        Node* sibling = x->children[idx - 1];

        child->keys.insert(child->keys.begin(), x->keys[idx - 1]);
        child->values.insert(child->values.begin(), x->values[idx - 1]);
        if (!child->isLeaf)
            child->children.insert(child->children.begin(), sibling->children[sibling->n]);
        
        x->keys[idx - 1] = sibling->keys[sibling->n - 1];
        x->values[idx - 1] = sibling->values[sibling->n - 1];

        child->n += 1;
        sibling->n -= 1;
        sibling->keys.resize(sibling->n);
        sibling->values.resize(sibling->n);
        if (!sibling->isLeaf) sibling->children.resize(sibling->n + 1);
    }

    void borrowFromNext(Node* x, int idx) {
        Node* child = x->children[idx];
        Node* sibling = x->children[idx + 1];

        child->keys.push_back(x->keys[idx]);
        child->values.push_back(x->values[idx]);
        if (!child->isLeaf)
            child->children.push_back(sibling->children[0]);

        x->keys[idx] = sibling->keys[0];
        x->values[idx] = sibling->values[0];

        child->n += 1;
        sibling->keys.erase(sibling->keys.begin());
        sibling->values.erase(sibling->values.begin());
        if (!sibling->isLeaf)
            sibling->children.erase(sibling->children.begin());
        sibling->n -= 1;
    }

    void merge(Node* x, int idx) {
        Node* child = x->children[idx];
        Node* sibling = x->children[idx + 1];

        child->keys.push_back(x->keys[idx]);
        child->values.push_back(x->values[idx]);

        for (int i = 0; i < sibling->n; ++i) {
            child->keys.push_back(sibling->keys[i]);
            child->values.push_back(sibling->values[i]);
        }
        if (!child->isLeaf) {
            for (int i = 0; i <= sibling->n; ++i)
                child->children.push_back(sibling->children[i]);
        }

        child->n += sibling->n + 1;
        
        x->keys.erase(x->keys.begin() + idx);
        x->values.erase(x->values.begin() + idx);
        x->children.erase(x->children.begin() + idx + 1);
        x->n--;

        delete sibling;
    }

public:
    // ... insert/search ...

    void remove(KeyType k) {
        if (!root) return;

        remove(root, k);

        if (root->n == 0) {
            Node* tmp = root;
            if (root->isLeaf)
                root = nullptr;
            else
                root = root->children[0];
            delete tmp;
        }
    }

    ValueType* search(KeyType k) {
        return (root == nullptr) ? nullptr : search(root, k);
    }

    void traverse() {
        if (root != nullptr) traverse(root);
    }

    void collectKeys(vector<KeyType>& result) {
        if (root != nullptr) collectKeys(root, result);
    }

    void collectValues(vector<ValueType>& result) {
        if (root != nullptr) collectValues(root, result);
    }

    // Serialization
    void saveNode(Node* x, ofstream& out) {
        out.write((char*)&x->isLeaf, sizeof(bool));
        out.write((char*)&x->n, sizeof(int));
        for (int i = 0; i < x->n; i++) {
            writeItem(out, x->keys[i]);
            writeItem(out, x->values[i]);
        }
        if (!x->isLeaf) {
            for (int i = 0; i <= x->n; i++) {
                saveNode(x->children[i], out);
            }
        }
    }

    Node* loadNode(ifstream& in) {
        bool isLeaf;
        int n;
        in.read((char*)&isLeaf, sizeof(bool));
        in.read((char*)&n, sizeof(int));

        Node* x = new Node(isLeaf);
        x->n = n;
        
        for (int i = 0; i < n; i++) {
            KeyType k;
            ValueType v;
            readItem(in, k);
            readItem(in, v);
            x->keys.push_back(k);
            x->values.push_back(v);
        }

        if (!isLeaf) {
            for (int i = 0; i <= n; i++) {
                x->children.push_back(loadNode(in));
            }
        }
        return x;
    }


    // ... insert/search ...

    void serialize(const string& filename) {
        ofstream out(filename, ios::binary);
        if (!out) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }
        out.write((char*)&t, sizeof(int));
        bool hasRoot = (root != nullptr);
        out.write((char*)&hasRoot, sizeof(bool));
        if (hasRoot)
            saveNode(root, out);
        out.close();
    }

    void deserialize(const string& filename) {
        ifstream in(filename, ios::binary);
        if (!in) {
            cerr << "Error opening file for reading: " << filename << endl;
            return;
        }
        in.read((char*)&t, sizeof(int));
        bool hasRoot;
        in.read((char*)&hasRoot, sizeof(bool));
        if (hasRoot)
            root = loadNode(in);
        else
            root = nullptr;
        in.close();
    }
};

#endif
