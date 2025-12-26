#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <functional>
#include <string>

using namespace std;

template<typename K, typename V>
class HashTable {
private:
    static const size_t DEFAULT_SIZE = 101;  // Prime number for better distribution
    vector<list<pair<K, V>>> buckets;
    size_t tableSize;
    size_t numElements;
    hash<K> hashFunction;
    
    size_t getHash(const K& key) const {
        return hashFunction(key) % tableSize;
    }
    
public:
    HashTable(size_t size = DEFAULT_SIZE);
    ~HashTable();
    
    // Insert or update key-value pair
    void insert(const K& key, const V& value);
    
    // Get value by key (returns nullptr if not found)
    V* get(const K& key);
    
    // Check if key exists
    bool contains(const K& key) const;
    
    // Remove key-value pair
    bool remove(const K& key);
    
    // Update existing key's value
    bool update(const K& key, const V& value);
    
    // Get all key-value pairs
    vector<pair<K, V>> getAllPairs() const;
    
    // Get number of elements
    size_t size() const { return numElements; }
    
    // Check if empty
    bool isEmpty() const { return numElements == 0; }
    
    // Clear all data
    void clear();
    
    // Save to file
    bool saveToFile(const string& filename);
    
    // Load from file
    bool loadFromFile(const string& filename);
};

// ==================== HashTable Implementation ====================

template<typename K, typename V>
HashTable<K, V>::HashTable(size_t size) : tableSize(size), numElements(0) {
    buckets.resize(tableSize);
}

template<typename K, typename V>
HashTable<K, V>::~HashTable() {
    clear();
}

template<typename K, typename V>
void HashTable<K, V>::insert(const K& key, const V& value) {
    size_t index = getHash(key);
    
    // Check if key already exists
    for (auto& pair : buckets[index]) {
        if (pair.first == key) {
            pair.second = value;  // Update existing
            return;
        }
    }
    
    // Insert new pair
    buckets[index].push_back({key, value});
    numElements++;
}

template<typename K, typename V>
V* HashTable<K, V>::get(const K& key) {
    size_t index = getHash(key);
    
    for (auto& pair : buckets[index]) {
        if (pair.first == key) {
            return &pair.second;
        }
    }
    
    return nullptr;
}

template<typename K, typename V>
bool HashTable<K, V>::contains(const K& key) const {
    size_t index = getHash(key);
    
    for (const auto& pair : buckets[index]) {
        if (pair.first == key) {
            return true;
        }
    }
    
    return false;
}

template<typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    size_t index = getHash(key);
    
    auto& bucket = buckets[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            numElements--;
            return true;
        }
    }
    
    return false;
}

template<typename K, typename V>
bool HashTable<K, V>::update(const K& key, const V& value) {
    V* found = get(key);
    if (found != nullptr) {
        *found = value;
        return true;
    }
    return false;
}

template<typename K, typename V>
vector<pair<K, V>> HashTable<K, V>::getAllPairs() const {
    vector<pair<K, V>> pairs;
    
    for (const auto& bucket : buckets) {
        for (const auto& pair : bucket) {
            pairs.push_back(pair);
        }
    }
    
    return pairs;
}

template<typename K, typename V>
void HashTable<K, V>::clear() {
    for (auto& bucket : buckets) {
        bucket.clear();
    }
    numElements = 0;
}

template<typename K, typename V>
bool HashTable<K, V>::saveToFile(const string& filename) {
    ofstream out(filename, ios::binary);
    if (!out.is_open()) return false;
    
    // Save table size and element count
    out.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));
    out.write(reinterpret_cast<const char*>(&numElements), sizeof(numElements));
    
    // Save all key-value pairs
    for (const auto& bucket : buckets) {
        for (const auto& pair : bucket) {
            out.write(reinterpret_cast<const char*>(&pair.first), sizeof(K));
            out.write(reinterpret_cast<const char*>(&pair.second), sizeof(V));
        }
    }
    
    out.close();
    return true;
}

template<typename K, typename V>
bool HashTable<K, V>::loadFromFile(const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) return false;
    
    clear();
    
    size_t savedTableSize, savedNumElements;
    in.read(reinterpret_cast<char*>(&savedTableSize), sizeof(savedTableSize));
    in.read(reinterpret_cast<char*>(&savedNumElements), sizeof(savedNumElements));
    
    // Resize if necessary
    if (savedTableSize != tableSize) {
        tableSize = savedTableSize;
        buckets.clear();
        buckets.resize(tableSize);
    }
    
    // Load all pairs
    for (size_t i = 0; i < savedNumElements; i++) {
        K key;
        V value;
        in.read(reinterpret_cast<char*>(&key), sizeof(K));
        in.read(reinterpret_cast<char*>(&value), sizeof(V));
        insert(key, value);
    }
    
    in.close();
    return true;
}

#endif // HASHTABLE_H
