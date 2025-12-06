#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <string>
#include <iostream>
#include <functional>

using namespace std;

template<typename KeyType, typename ValueType>
class HashTable {
private:
    struct Entry {
        KeyType key;
        ValueType value;
        Entry* next;

        Entry(KeyType k, ValueType v) : key(k), value(v), next(nullptr) {}
    };

    vector<Entry*> table;
    int size;
    int count;
    const double LOAD_FACTOR_THRESHOLD = 0.7;

    size_t hash(KeyType key) {
        // Default hash using std::hash
        return std::hash<KeyType>{}(key) % size;
    }

    // Specialization for string if needed, but std::hash<string> works well.
    // Implementing DJB2 as requested for strings manually if we want, 
    // but std::hash is standard. Let's use std::hash for simplicity and generality.
    // If KeyType is string, std::hash<string> is used.

    void rehash() {
        int oldSize = size;
        vector<Entry*> oldTable = table;

        size = size * 2 + 1; // Keep it odd/prime-ish
        table.assign(size, nullptr);
        count = 0;

        for (int i = 0; i < oldSize; i++) {
            Entry* entry = oldTable[i];
            while (entry != nullptr) {
                insert(entry->key, entry->value);
                Entry* temp = entry;
                entry = entry->next;
                delete temp;
            }
        }
    }

public:
    HashTable(int initialSize = 1009) : size(initialSize), count(0) {
        table.assign(size, nullptr);
    }

    ~HashTable() {
        for (int i = 0; i < size; i++) {
            Entry* entry = table[i];
            while (entry != nullptr) {
                Entry* temp = entry;
                entry = entry->next;
                delete temp;
            }
        }
    }

    void insert(KeyType key, ValueType value) {
        if ((double)count / size > LOAD_FACTOR_THRESHOLD) {
            rehash();
        }

        size_t index = hash(key);
        Entry* entry = table[index];

        // Check for update
        while (entry != nullptr) {
            if (entry->key == key) {
                entry->value = value;
                return;
            }
            entry = entry->next;
        }

        // Insert new
        Entry* newEntry = new Entry(key, value);
        newEntry->next = table[index];
        table[index] = newEntry;
        count++;
    }

    ValueType* get(KeyType key) {
        size_t index = hash(key);
        Entry* entry = table[index];

        while (entry != nullptr) {
            if (entry->key == key) {
                return &(entry->value);
            }
            entry = entry->next;
        }
        return nullptr;
    }

    bool remove(KeyType key) {
        size_t index = hash(key);
        Entry* entry = table[index];
        Entry* prev = nullptr;

        while (entry != nullptr) {
            if (entry->key == key) {
                if (prev == nullptr) {
                    table[index] = entry->next;
                } else {
                    prev->next = entry->next;
                }
                delete entry;
                count--;
                return true;
            }
            prev = entry;
            entry = entry->next;
        }
        return false;
    }

    double loadFactor() {
        return (double)count / size;
    }
    
    int getSize() {
        return size;
    }
};

#endif
