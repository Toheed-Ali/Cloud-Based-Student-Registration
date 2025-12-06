#include "../src/database/database.h"
#include <iostream>
#include <cassert>

using namespace std;

void testDatabase() {
    cout << "Testing Database..." << endl;
    Database db;
    
    // Test Student
    Student s = {"2021-CS-001", "Test Student", "test@test.com", "password", 1, 0.0, STUDENT};
    assert(db.addStudent(s) == true);
    assert(db.addStudent(s) == false); // Duplicate
    
    Student* retrieved = db.getStudent("2021-CS-001");
    assert(retrieved != nullptr);
    assert(retrieved->name == "Test Student");
    
    // Test Auth
    UserAuth* auth = db.authenticate("2021-CS-001", "password");
    assert(auth != nullptr);
    assert(auth->role == STUDENT);
    assert(db.authenticate("2021-CS-001", "wrong") == nullptr);
    
    // Test Course
    Course c = {"CS101", "Programming", 1, "teacher", 3, 50, 0};
    assert(db.addCourse(c) == true);
    
    // Test Enrollment
    assert(db.enrollStudent("2021-CS-001", "CS101", 1) == true);
    
    cout << "Database Operations Passed!" << endl;
}

void testPersistence() {
    cout << "Testing Persistence..." << endl;
    {
        Database db;
        Student s = {"2021-CS-999", "Persistent Student", "p@p.com", "pass", 1, 0.0, STUDENT};
        db.addStudent(s);
        db.save();
    }
    
    {
        Database db;
        db.load();
        Student* s = db.getStudent("2021-CS-999");
        assert(s != nullptr);
        assert(s->name == "Persistent Student");
    }
    cout << "Persistence Passed!" << endl;
}

int main() {
    testDatabase();
    testPersistence();
    cout << "All Database Tests Passed!" << endl;
    return 0;
}
