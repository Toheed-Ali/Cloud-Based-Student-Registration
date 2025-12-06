#include "database.h"
#include <iostream>

Database::Database() : studentTree(5), teacherTree(5), courseTree(5), enrollmentTree(5), authTable(1009) {
    // Constructor
}

Database::~Database() {
    // Destructor
}

bool Database::addStudent(const Student& s) {
    if (studentTree.search(s.rollNumber) != nullptr) {
        return false; // Already exists
    }
    studentTree.insert(s.rollNumber, s);
    
    // Add to Auth Table
    UserAuth auth;
    auth.username = s.rollNumber; // Students use RollNo as username
    auth.passwordHash = s.passwordHash;
    auth.role = STUDENT;
    auth.referenceID = s.rollNumber;
    authTable.insert(auth.username, auth);
    
    return true;
}

Student* Database::getStudent(const string& rollNo) {
    return studentTree.search(rollNo);
}

bool Database::addTeacher(const Teacher& t) {
    if (teacherTree.search(t.username) != nullptr) {
        return false;
    }
    teacherTree.insert(t.username, t);
    
    UserAuth auth;
    auth.username = t.username;
    auth.passwordHash = t.passwordHash;
    auth.role = TEACHER;
    auth.referenceID = t.username;
    authTable.insert(auth.username, auth);
    
    return true;
}

Teacher* Database::getTeacher(const string& username) {
    return teacherTree.search(username);
}

bool Database::addCourse(const Course& c) {
    if (courseTree.search(c.courseCode) != nullptr) {
        return false;
    }
    courseTree.insert(c.courseCode, c);
    return true;
}

Course* Database::getCourse(const string& code) {
    return courseTree.search(code);
}

bool Database::enrollStudent(const string& rollNo, const string& courseCode, int semester) {
    Student* s = getStudent(rollNo);
    Course* c = getCourse(courseCode);
    
    if (!s || !c) return false;
    
    string enrollmentID = rollNo + "_" + courseCode;
    if (enrollmentTree.search(enrollmentID) != nullptr) {
        return false; // Already enrolled
    }
    
    Enrollment e;
    e.enrollmentID = enrollmentID;
    e.studentRollNo = rollNo;
    e.courseCode = courseCode;
    e.semesterTaken = semester;
    e.grade = "IP";
    
    enrollmentTree.insert(enrollmentID, e);
    
    // Update course count (not implemented in BTree value update yet, need to re-insert)
    c->enrolledCount++;
    // courseTree.insert(c->courseCode, *c); // Overwrite? BTree insert might not overwrite if key exists logic isn't "update"
    // My BTree insert ignores if key exists in leaf splitting logic? 
    // Actually my BTree insert logic:
    // insertNonFull: if leaf, insert sorted.
    // It doesn't check for duplicates explicitly in `insertNonFull`.
    // But `addStudent` checks `search` first.
    // If I call `insert` again with same key, it might duplicate it in the node!
    // I should implement update or check in insert.
    // For now, I will assume insert doesn't update.
    // I need to implement update.
    
    return true;
}

UserAuth* Database::authenticate(const string& username, const string& password) {
    UserAuth* auth = authTable.get(username);
    if (auth && auth->passwordHash == password) {
        return auth;
    }
    return nullptr;
}

void Database::save() {
    studentTree.serialize("students.dat");
    teacherTree.serialize("teachers.dat");
    courseTree.serialize("courses.dat");
    enrollmentTree.serialize("enrollments.dat");
}

void Database::load() {
    studentTree.deserialize("students.dat");
    teacherTree.deserialize("teachers.dat");
    courseTree.deserialize("courses.dat");
    enrollmentTree.deserialize("enrollments.dat");
}

// Helper to traverse BTree and collect keys/values
// Since BTree traverse prints, we need a new method or a friend class or just a simple workaround.
// For this project, let's add a `collect` method to BTree or just implement a traversal here if we had access.
// But `root` is private.
// I will add `collect` to BTree<Key, Value>.

vector<string> Database::getAllCourseCodes() {
    vector<string> codes;
    courseTree.collectKeys(codes);
    return codes;
}

vector<Enrollment> Database::getAllEnrollments() {
    vector<Enrollment> enrollments;
    enrollmentTree.collectValues(enrollments);
    return enrollments;
}

void Database::generateSampleData() {
    // Add Admin User
    UserAuth admin;
    admin.username = "admin";
    admin.passwordHash = "admin123";
    admin.role = ADMIN;
    admin.referenceID = "admin";
    authTable.insert(admin.username, admin);
    
    // Add Students
    Student s1 = {"2021-CS-001", "Ahmed Ali", "ahmed@test.com", "pass123", 1, 3.5, STUDENT};
    addStudent(s1);
    
    // Add Teachers
    Teacher t1 = {"prof_khan", "Dr. Khan", "khan@test.com", "teach123", TEACHER};
    addTeacher(t1);
    
    // Add Courses
    Course c1 = {"CS101", "Intro to CS", 1, "prof_khan", 3, 60, 0};
    addCourse(c1);
}
