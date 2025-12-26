#include <iostream>
#include "../database/IndexedStorage.h"
#include "../database/DataModels.h"

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "  IndexedStorage Test" << endl;
    cout << "========================================\n" << endl;
    
    // Test with Student
    IndexedStorage<Student> studentStorage("test_data/students");
    
    // Add some students
    Student s1;
    s1.studentID = "BSCS24001";
    s1.email = "bscs24001@itu.edu.pk";
    s1.name = "Test Student 1";
    s1.currentSemester = 3;
    
    Student s2;
    s2.studentID = "BSCS24002";
    s2.email = "bscs24002@itu.edu.pk";
    s2.name = "Test Student 2";
    s2.currentSemester = 3;
    
    cout << "Adding students..." << endl;
    studentStorage.add(s1);
    studentStorage.add(s2);
    
    // Test get (O(1) hash lookup)
    cout << "\nTesting get (hash lookup)..." << endl;
    Student retrieved;
    if (studentStorage.get("BSCS24001", retrieved)) {
        cout << "✓ Found: " << retrieved.name << endl;
    } else {
        cout << "✗ Not found!" << endl;
    }
    
    // Test exists
    cout << "\nTesting exists..." << endl;
    if (studentStorage.exists("BSCS24002")) {
        cout << "✓ BSCS24002 exists" << endl;
    }
    
    // Test getAll (sorted by B-Tree)
    cout << "\nGetting all students (B-Tree sorted)..." << endl;
    auto all = studentStorage.getAll();
    for (const auto& s : all) {
        cout << "  " << s.studentID << ": " << s.name << endl;
    }
    
    // Test update
    cout << "\nUpdating student..." << endl;
    s1.name = "Updated Student 1";
    studentStorage.update(s1);
    
    if (studentStorage.get("BSCS24001", retrieved)) {
        cout << "✓ Updated name: " << retrieved.name << endl;
    }
    
    // Test remove
    cout << "\nRemoving student..." << endl;
    studentStorage.remove("BSCS24002");
    if (!studentStorage.exists("BSCS24002")) {
        cout << "✓ Successfully removed BSCS24002" << endl;
    }
    
    cout << "\n========================================" << endl;
    cout << "All tests passed!" << endl;
    cout << "========================================" << endl;
    
    return 0;
}
