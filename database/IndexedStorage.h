#ifndef INDEXED_STORAGE_H
#define INDEXED_STORAGE_H

#include "BTree.h"
#include "HashTable.h"
#include "DataModels.h"
#include <fstream>
#include <iostream>
#include <type_traits>  // for is_same_v and if constexpr

using namespace std;

/**
 * IndexedStorage - Combines B-Tree and Hash Table for optimal performance
 * 
 * - B-Tree: For sorted iteration and range queries
 * - HashTable: For O(1) get/exists lookups
 * - Data File: For actual entity storage
 * 
 * Template specializations for Student, Course, Teacher, User
 */
template<typename T>
class IndexedStorage {
private:
    BTree<string, size_t> btree;          // ID -> file offset
    HashTable<string, size_t> hashTable;  // ID -> file offset
    string dataFilename;
    string btreeFilename;
    string hashFilename;
    
    // Get entity ID (must specialize for each type)
    string getID(const T& entity);
    
    // Write entity to data file, return offset
    size_t writeEntity(const T& entity, size_t offset = 0);
    
    // Read entity from data file at offset
    bool readEntity(size_t offset, T& entity);
    
public:
    IndexedStorage(const string& baseName);
    ~IndexedStorage();
    
    // Core operations
    bool add(const T& entity);
    bool get(const string& id, T& entity);
    bool update(const T& entity);
    bool remove(const string& id);
    bool exists(const string& id);
    
    // Get all entities (sorted by ID via B-Tree)
    vector<T> getAll();
    
    // Persistence
    void save();
    void load();
    void clear();
};

// ==================== Implementation ====================

template<typename T>
IndexedStorage<T>::IndexedStorage(const string& baseName) 
    : dataFilename(baseName + ".dat"),
      btreeFilename(baseName + ".btree"),
      hashFilename(baseName + ".hash") {
    
    load();  // Load existing data if present
}

template<typename T>
IndexedStorage<T>::~IndexedStorage() {
    save();  // Auto-save on destruction
}

template<typename T>
bool IndexedStorage<T>::add(const T& entity) {
    string id = getID(entity);
    
    // Check if already exists
    if (hashTable.contains(id)) {
        return update(entity);  // Update instead
    }
    
    // Write entity to data file
    size_t offset = writeEntity(entity);
    
    // Add to both indexes
    btree.insert(id, offset);
    hashTable.insert(id, offset);
    
    return true;
}

template<typename T>
bool IndexedStorage<T>::get(const string& id, T& entity) {
    // Use hash table for O(1) lookup
    size_t* offsetPtr = hashTable.get(id);
    if (offsetPtr == nullptr) {
        return false;
    }
    
    // Read from data file
    return readEntity(*offsetPtr, entity);
}

template<typename T>
bool IndexedStorage<T>::update(const T& entity) {
    string id = getID(entity);
    
    // Get existing offset
    size_t* offsetPtr = hashTable.get(id);
    if (offsetPtr == nullptr) {
        return false;  // Doesn't exist
    }
    
    // Overwrite at same offset
    writeEntity(entity, *offsetPtr);
    
    return true;
}

template<typename T>
bool IndexedStorage<T>::remove(const string& id) {
    // Remove from both indexes
    bool removed1 = hashTable.remove(id);
    btree.remove(id);
    
    // Note: We don't actually delete from data file (space will be reused)
    return removed1;
}

template<typename T>
bool IndexedStorage<T>::exists(const string& id) {
    return hashTable.contains(id);
}

template<typename T>
vector<T> IndexedStorage<T>::getAll() {
    vector<T> results;
    
    // Get all ID-offset pairs from B-Tree (sorted)
    auto pairs = btree.getAllPairs();
    
    for (const auto& pair : pairs) {
        T entity;
        if (readEntity(pair.second, entity)) {
            results.push_back(entity);
        }
    }
    
    return results;
}

template<typename T>
void IndexedStorage<T>::save() {
    btree.saveToFile(btreeFilename);
    hashTable.saveToFile(hashFilename);
    // Data file is written incrementally, no need to save again
}

template<typename T>
void IndexedStorage<T>::load() {
    btree.loadFromFile(btreeFilename);
    hashTable.loadFromFile(hashFilename);
}

template<typename T>
void IndexedStorage<T>::clear() {
    btree.clear();
    hashTable.clear();
    // Optionally delete data file
    remove(dataFilename.c_str());
}

// ==================== File I/O Helpers ====================

template<typename T>
size_t IndexedStorage<T>::writeEntity(const T& entity, size_t offset) {
    fstream file;
    
    if (offset == 0) {
        // Append to end
        file.open(dataFilename, ios::in | ios::out | ios::binary | ios::ate);
        if (!file.is_open()) {
            // Create new file
            file.open(dataFilename, ios::out | ios::binary);
            file.close();
            file.open(dataFilename, ios::in | ios::out | ios::binary | ios::ate);
        }
        offset = file.tellp();
    } else {
        // Overwrite at specific offset
        file.open(dataFilename, ios::in | ios::out | ios::binary);
        file.seekp(offset);
    }
    
    if (!file.is_open()) {
        cerr << "Failed to open data file: " << dataFilename << endl;
        return 0;
    }
    
    // Write entity size first
    size_t entitySize = sizeof(T);
    file.write(reinterpret_cast<const char*>(&entitySize), sizeof(size_t));
    
    // Write entity data
    file.write(reinterpret_cast<const char*>(&entity), sizeof(T));
    
    file.close();
    return offset;
}

template<typename T>
bool IndexedStorage<T>::readEntity(size_t offset, T& entity) {
    ifstream file(dataFilename, ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(offset);
    
    // Read entity size
    size_t entitySize;
    file.read(reinterpret_cast<char*>(&entitySize), sizeof(size_t));
    
    // Read entity data
    file.read(reinterpret_cast<char*>(&entity), sizeof(T));
    
    file.close();
    return true;
}

// ==================== Helper for static_assert ====================
// Must be defined BEFORE getID implementation

template<typename> struct always_false : std::false_type {};

// ==================== getID implementation using if constexpr ====================

template<typename T>
string IndexedStorage<T>::getID(const T& entity) {
    if constexpr (std::is_same_v<T, Student>) {
        return entity.studentID;
    } else if constexpr (std::is_same_v<T, Course>) {
        return entity.courseID;
    } else if constexpr (std::is_same_v<T, Teacher>) {
        return entity.teacherID;
    } else if constexpr (std::is_same_v<T, User>) {
        return entity.userID;
    } else if constexpr (std::is_same_v<T, Timetable>) {
        return std::to_string(entity.semesterNumber);
    } else {
        static_assert(always_false<T>::value, "getID not implemented for this type");
        return "";
    }
}

#endif // INDEXED_STORAGE_H
