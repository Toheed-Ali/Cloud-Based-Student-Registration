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
    
    // Write entity to data file, return offset (line number)
    size_t writeEntity(const T& entity, size_t offset = 0);
    
    // Read entity from data file at offset (line number)
    bool readEntity(size_t offset, T& entity);
    
    // Serialization helpers (use existing Serialization.h)
    string serializeEntity(const T& entity);
    T deserializeEntity(const string& data);
    
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

// ==================== File I/O Helpers (TEXT-BASED for portability) ====================

template<typename T>
size_t IndexedStorage<T>::writeEntity(const T& entity, size_t offset) {
    // Use text-based line storage for portability
    // Each entity is one line in the file
    
    // Read all existing lines
    vector<string> lines;
    ifstream inFile(dataFilename);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();
    }
    
    // Serialize the entity to text
    string serialized = serializeEntity(entity);
    
    if (offset == 0) {
        // Append new entity
        offset = lines.size();
        lines.push_back(serialized);
    } else {
        // Update existing entity at offset (offset is line number)
        if (offset < lines.size()) {
            lines[offset] = serialized;
        } else {
            // Shouldn't happen, but handle gracefully
            offset = lines.size();
            lines.push_back(serialized);
        }
    }
    
    // Write all lines back
    ofstream outFile(dataFilename);
    if (!outFile.is_open()) {
        cerr << "Failed to open data file for writing: " << dataFilename << endl;
        return 0;
    }
    
    for (const auto& line : lines) {
        outFile << line << "\n";
    }
    outFile.close();
    
    return offset;
}

template<typename T>
bool IndexedStorage<T>::readEntity(size_t offset, T& entity) {
    ifstream file(dataFilename);
    if (!file.is_open()) {
        return false;
    }
    
    // offset is line number
    string line;
    size_t currentLine = 0;
    while (getline(file, line)) {
        if (currentLine == offset) {
            file.close();
            entity = deserializeEntity(line);
            return true;
        }
        currentLine++;
    }
    
    file.close();
    return false;
}

// ==================== Serialization Helpers (using existing Serialization.h) ====================

#include "Serialization.h"

template<typename T>
string IndexedStorage<T>::serializeEntity(const T& entity) {
    if constexpr (is_same_v<T, Student>) {
        return Serializer::serializeStudent(entity);
    } else if constexpr (is_same_v<T, Course>) {
        return Serializer::serializeCourse(entity);
    } else if constexpr (is_same_v<T, Teacher>) {
        return Serializer::serializeTeacher(entity);
    } else if constexpr (is_same_v<T, User>) {
        return Serializer::serializeUser(entity);
    } else if constexpr (is_same_v<T, Timetable>) {
        return Serializer::serializeTimetable(entity);
    } else {
        return "";
    }
}

template<typename T>
T IndexedStorage<T>::deserializeEntity(const string& data) {
    if constexpr (is_same_v<T, Student>) {
        return Serializer::deserializeStudent(data);
    } else if constexpr (is_same_v<T, Course>) {
        return Serializer::deserializeCourse(data);
    } else if constexpr (is_same_v<T, Teacher>) {
        return Serializer::deserializeTeacher(data);
    } else if constexpr (is_same_v<T, User>) {
        return Serializer::deserializeUser(data);
    } else if constexpr (is_same_v<T, Timetable>) {
        return Serializer::deserializeTimetable(data);
    } else {
        return T();
    }
}

// ==================== getID implementation using if constexpr ====================

// Helper for static_assert (must be defined BEFORE use)
template<typename> struct always_false : std::false_type {};

template<typename T>
string IndexedStorage<T>::getID(const T& entity) {
    if constexpr (is_same_v<T, Student>) {
        return entity.studentID;
    } else if constexpr (is_same_v<T, Course>) {
        return entity.courseID;
    } else if constexpr (is_same_v<T, Teacher>) {
        return entity.teacherID;
    } else if constexpr (is_same_v<T, User>) {
        return entity.userID;
    } else if constexpr (is_same_v<T, Timetable>) {
        return to_string(entity.semesterNumber);
    } else {
        static_assert(always_false<T>::value, "getID not implemented for this type");
        return "";
    }
}

#endif // INDEXED_STORAGE_H
