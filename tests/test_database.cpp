#include <iostream>
#include <cassert>
#include "../database/DatabaseManager.h"

using namespace std;

void testUserOperations(DatabaseManager& db) {
    cout << "\n=== Testing User Operations ===" << endl;
    
    // Create user
    User testUser;
    testUser.userID = "U001";
    testUser.email = "test@university.com";
    testUser.passwordHash = "password123";
    testUser.role = UserRole::STUDENT;
    testUser.name = "Test User";
    
    assert(db.createUser(testUser));
    cout << "[PASS] User creation" << endl;
    
    // Authenticate
    User authUser;
    assert(db.authenticateUser("test@university.com", "password123", authUser));
    assert(authUser.name == "Test User");
    cout << "[PASS] User authentication" << endl;
    
    // Get user by email
    User* retrieved = db.getUserByEmail("test@university.com");
    assert(retrieved != nullptr);
    assert(retrieved->email == "test@university.com");
    cout << "[PASS] Get user by email" << endl;
}

void testStudentOperations(DatabaseManager& db) {
    cout << "\n=== Testing Student Operations ===" << endl;
    
    // Create student
    Student student;
    student.studentID = "S001";
    student.email = "student1@university.com";
    student.name = "John Doe";
    student.currentSemester = 1;
    student.contactInfo = "123-456-7890";
    student.dateOfAdmission = time(nullptr);
    
    assert(db.addStudent(student));
    cout << "[PASS] Student creation" << endl;
    
    // Retrieve student
    Student* retrieved = db.getStudent("S001");
    assert(retrieved != nullptr);
    assert(retrieved->name == "John Doe");
    assert(retrieved->currentSemester == 1);
    cout << "[PASS] Get student" << endl;
    
    // Update student - make a copy to avoid static pointer issues
    Student studentCopy = *retrieved;
    studentCopy.currentSemester = 2;
    assert(db.updateStudent(studentCopy));
    Student* updated = db.getStudent("S001");
    assert(updated->currentSemester == 2);
    cout << "[PASS] Update student" << endl;
}

void testTeacherOperations(DatabaseManager& db) {
    cout << "\n=== Testing Teacher Operations ===" << endl;
    
    Teacher teacher;
    teacher.teacherID = "T001";
    teacher.email = "teacher1@university.com";
    teacher.name = "Dr. Smith";
    teacher.assignedCourseID = "CS101";
    teacher.department = "Computer Science";
    teacher.contactInfo = "987-654-3210";
    
    assert(db.addTeacher(teacher));
    cout << "[PASS] Teacher creation" << endl;
    
    Teacher* retrieved = db.getTeacher("T001");
    assert(retrieved != nullptr);
    assert(retrieved->name == "Dr. Smith");
    cout << "[PASS] Get teacher" << endl;
}

void testCourseOperations(DatabaseManager& db) {
    cout << "\n=== Testing Course Operations ===" << endl;
    
    Course course;
    course.courseID = "CS101";
    course.courseName = "Introduction to Programming";
    course.semester = 1;
    course.teacherID = "T001";
    course.currentEnrollmentCount = 0;
    
    assert(db.addCourse(course));
    cout << "[PASS] Course creation" << endl;
    
    Course* retrieved = db.getCourse("CS101");
    assert(retrieved != nullptr);
    assert(retrieved->courseName == "Introduction to Programming");
    cout << "[PASS] Get course" << endl;
}

void testEnrollmentOperations(DatabaseManager& db) {
    cout << "\n=== Testing Enrollment Operations ===" << endl;
    
    // Set registration window to be open
    SystemConfig config = db.getConfig();
    config.isRegistrationOpen = true;
    config.registrationStartTime = time(nullptr) - 3600;  // 1 hour ago
    config.registrationEndTime = time(nullptr) + 3600;    // 1 hour from now
    db.updateConfig(config);
    
    // Enroll student in course
    bool enrolled = db.enrollStudent("S001", "CS101");
    assert(enrolled);
    cout << "[PASS] Student enrollment" << endl;
    
    // Verify enrollment
    Student* student = db.getStudent("S001");
    assert(student->enrolledCourses.size() == 1);
    assert(student->enrolledCourses[0] == "CS101");
    
    Course* course = db.getCourse("CS101");
    assert(course->currentEnrollmentCount == 1);
    assert(course->enrolledStudents[0] == "S001");
    cout << "[PASS] Enrollment verification" << endl;
    
    // Drop course
    bool dropped = db.dropCourse("S001", "CS101");
    assert(dropped);
    cout << "[PASS] Drop course" << endl;
    
    // Verify drop
    student = db.getStudent("S001");
    assert(student->enrolledCourses.empty());
    
    course = db.getCourse("CS101");
    assert(course->currentEnrollmentCount == 0);
    cout << "[PASS] Drop course verification" << endl;
}

void testPersistence(DatabaseManager& db) {
    cout << "\n=== Testing Persistence ===" << endl;
    
    // Save all
    assert(db.saveAll());
    cout << "[PASS] Save all data" << endl;
    
    // Create new database manager and load
    DatabaseManager db2("data");
    assert(db2.loadAll());
    cout << "[PASS] Load all data" << endl;
    
    // Verify data was persisted
    User* adminUser = db2.getUserByEmail("admin@university.com");
    assert(adminUser != nullptr);
    assert(adminUser->role == UserRole::ADMIN);
    
    Student* student = db2.getStudent("S001");
    assert(student != nullptr);
    assert(student->name == "John Doe");
    
    Teacher* teacher = db2.getTeacher("T001");
    assert(teacher != nullptr);
    assert(teacher->name == "Dr. Smith");
    
    Course* course = db2.getCourse("CS101");
    assert(course != nullptr);
    assert(course->courseName == "Introduction to Programming");
    
    cout << "[PASS] Data persistence verification" << endl;
}

void testBTree() {
    cout << "\n=== Testing B-Tree ===" << endl;
    
    BTree<int, string> btree;
    
    // Insert
    btree.insert(10, "ten");
    btree.insert(20, "twenty");
    btree.insert(5, "five");
    btree.insert(15, "fifteen");
    btree.insert(25, "twenty-five");
    cout << "[PASS] B-Tree insertion" << endl;
    
    // Search
    string* val = btree.search(15);
    assert(val != nullptr);
    assert(*val == "fifteen");
    cout << "[PASS] B-Tree search" << endl;
    
    // Update
    assert(btree.update(15, "FIFTEEN"));
    val = btree.search(15);
    assert(*val == "FIFTEEN");
    cout << "[PASS] B-Tree update" << endl;
    
    // Remove
    btree.remove(15);
    val = btree.search(15);
    assert(val == nullptr);
    cout << "[PASS] B-Tree removal" << endl;
    
    // Get all pairs
    auto pairs = btree.getAllPairs();
    assert(pairs.size() == 4);  // 5 inserted, 1 removed
    cout << "[PASS] B-Tree get all pairs" << endl;
}

void testHashTable() {
    cout << "\n=== Testing Hash Table ===" << endl;
    
    HashTable<string, int> hashtable;
    
    // Insert
    hashtable.insert("apple", 1);
    hashtable.insert("banana", 2);
    hashtable.insert("cherry", 3);
    cout << "[PASS] Hash Table insertion" << endl;
    
    // Get
    int* val = hashtable.get("banana");
    assert(val != nullptr);
    assert(*val == 2);
    cout << "[PASS] Hash Table get" << endl;
    
    // Contains
    assert(hashtable.contains("apple"));
    assert(!hashtable.contains("orange"));
    cout << "[PASS] Hash Table contains" << endl;
    
    // Update
assert(hashtable.update("banana", 20));
    val = hashtable.get("banana");
    assert(*val == 20);
    cout << "[PASS] Hash Table update" << endl;
    
    // Remove
    assert(hashtable.remove("cherry"));
    assert(!hashtable.contains("cherry"));
    cout << "[PASS] Hash Table removal" << endl;
    
    // Size
    assert(hashtable.size() == 2);
    cout << "[PASS] Hash Table size" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  Database Layer Test Suite" << endl;
    cout << "========================================" << endl;
    
    try {
        // Test basic data structures
        testBTree();
        testHashTable();
        
        // Test database manager
        DatabaseManager db("data");
        db.initialize();
        
        testUserOperations(db);
        testStudentOperations(db);
        testTeacherOperations(db);
        testCourseOperations(db);
        testEnrollmentOperations(db);
        testPersistence(db);
        
        cout << "\n========================================" << endl;
        cout << "  ALL TESTS PASSED!" << endl;
        cout << "========================================" << endl;
        
        return 0;
    } catch (const exception& e) {
        cerr << "\n[ERROR] Test failed: " << e.what() << endl;
        return 1;
    }
}
