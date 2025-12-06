#include "../src/datastructures/btree.h"
#include <iostream>
#include <string>

using namespace std;

void testInsertAndSearch() {
    cout << "Testing Insert and Search..." << endl;
    BTree<int, string> tree(3); // Minimum degree 3

    tree.insert(10, "Ten");
    tree.insert(20, "Twenty");
    tree.insert(5, "Five");
    tree.insert(6, "Six");
    tree.insert(12, "Twelve");
    tree.insert(30, "Thirty");
    tree.insert(7, "Seven");
    tree.insert(17, "Seventeen");

    auto check = [&](int k, string expected) {
        string* res = tree.search(k);
        if (res == nullptr) {
            cout << "FAILED: Search(" << k << ") returned nullptr" << endl;
            cout << "Tree structure:" << endl;
            tree.traverse();
            exit(1);
        }
        if (*res != expected) {
            cout << "FAILED: Search(" << k << ") returned " << *res << ", expected " << expected << endl;
            exit(1);
        }
        cout << "Passed: Search(" << k << ")" << endl;
    };

    check(10, "Ten");
    check(6, "Six");
    check(30, "Thirty");
    
    if (tree.search(99) != nullptr) {
        cout << "FAILED: Search(99) should be nullptr" << endl;
        exit(1);
    }
    cout << "Passed: Search(99)" << endl;

    cout << "Insert and Search Passed!" << endl;
}

void testLargeInsert() {
    cout << "Testing Large Insert..." << endl;
    BTree<int, int> tree(3);
    for (int i = 0; i < 100; ++i) {
        tree.insert(i, i * 10);
    }

    for (int i = 0; i < 100; ++i) {
        int* val = tree.search(i);
        if (val == nullptr) {
            cout << "FAILED: Large Search(" << i << ") returned nullptr" << endl;
            exit(1);
        }
        if (*val != i * 10) {
            cout << "FAILED: Large Search(" << i << ") returned " << *val << ", expected " << i * 10 << endl;
            exit(1);
        }
    }
    cout << "Large Insert Passed!" << endl;
}

void testSerialization() {
    cout << "Testing Serialization..." << endl;
    {
        BTree<int, string> tree(3);
        tree.insert(1, "One");
        tree.insert(2, "Two");
        tree.insert(3, "Three");
        tree.serialize("test_btree.dat");
    }

    {
        BTree<int, string> tree(3);
        tree.deserialize("test_btree.dat");
        string* val = tree.search(2);
        if (val == nullptr || *val != "Two") {
            cout << "FAILED: Serialization check" << endl;
            exit(1);
        }
        cout << "Serialization Passed!" << endl;
    }
}

void testRemove() {
    cout << "Testing Remove..." << endl;
    BTree<int, string> tree(3);
    tree.insert(1, "One");
    tree.insert(2, "Two");
    tree.insert(3, "Three");
    tree.insert(4, "Four");
    tree.insert(5, "Five");
    tree.insert(6, "Six");

    tree.remove(3);
    if (tree.search(3) != nullptr) {
        cout << "FAILED: Remove(3) - Key still exists" << endl;
        exit(1);
    }
    if (*tree.search(2) != "Two" || *tree.search(4) != "Four") {
        cout << "FAILED: Remove(3) - Other keys damaged" << endl;
        exit(1);
    }

    tree.remove(6);
    if (tree.search(6) != nullptr) {
        cout << "FAILED: Remove(6) - Key still exists" << endl;
        exit(1);
    }

    cout << "Remove Passed!" << endl;
}

int main() {
    testInsertAndSearch();
    testLargeInsert();
    testSerialization();
    testRemove();
    cout << "All B-Tree Tests Passed!" << endl;
    return 0;
}
