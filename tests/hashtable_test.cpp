#include "../src/datastructures/hashtable.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

void testInsertAndGet() {
    cout << "Testing Insert and Get..." << endl;
    HashTable<string, int> ht;
    ht.insert("one", 1);
    ht.insert("two", 2);
    ht.insert("three", 3);

    assert(*ht.get("one") == 1);
    assert(*ht.get("two") == 2);
    assert(*ht.get("three") == 3);
    assert(ht.get("four") == nullptr);

    // Update
    ht.insert("one", 11);
    assert(*ht.get("one") == 11);

    cout << "Insert and Get Passed!" << endl;
}

void testRemove() {
    cout << "Testing Remove..." << endl;
    HashTable<string, int> ht;
    ht.insert("one", 1);
    ht.insert("two", 2);

    assert(ht.remove("one") == true);
    assert(ht.get("one") == nullptr);
    assert(ht.remove("one") == false);
    assert(*ht.get("two") == 2);

    cout << "Remove Passed!" << endl;
}

void testResizing() {
    cout << "Testing Resizing..." << endl;
    HashTable<int, int> ht(10); // Small size
    int initialSize = ht.getSize();

    for (int i = 0; i < 20; ++i) {
        ht.insert(i, i * 10);
    }

    assert(ht.getSize() > initialSize);
    for (int i = 0; i < 20; ++i) {
        assert(*ht.get(i) == i * 10);
    }

    cout << "Resizing Passed!" << endl;
}

int main() {
    testInsertAndGet();
    testRemove();
    testResizing();
    cout << "All Hash Table Tests Passed!" << endl;
    return 0;
}
