#ifndef DATABASE_H
#define DATABASE_H

#include "../datastructures/btree.h"
#include "../datastructures/hashtable.h"
#include <string>
#include <vector>
#include <ctime>

using namespace std;

enum UserRole { ADMIN, TEACHER, STUDENT };

struct Student {
    string rollNumber;
    string name;
    string email;
    string passwordHash;
    int currentSemester;
    // vector<string> failedCourses; // Simplified for now
    double cgpa;
    UserRole role = STUDENT;
};

struct Teacher {
    string username;
    string name;
    string email;
    string passwordHash;
    // vector<string> coursesTeaching;
    UserRole role = TEACHER;
};

struct Course {
    string courseCode;
    string courseName;
    int semester;
    string teacherUsername;
    int creditHours;
    int maxStudents;
    int enrolledCount;
};

struct Enrollment {
    string enrollmentID; // RollNo_CourseCode
    string studentRollNo;
    string courseCode;
    int semesterTaken;
    string grade; // "IP", "A", etc.
};

struct UserAuth {
    string username;
    string passwordHash;
    UserRole role;
    string referenceID; // RollNo or Username
};

class Database {
private:
    BTree<string, Student> studentTree;
    BTree<string, Teacher> teacherTree;
    BTree<string, Course> courseTree;
    BTree<string, Enrollment> enrollmentTree;
    
    HashTable<string, UserAuth> authTable;
    
    // Helper to load/save B-Trees
    void saveBTrees();
    void loadBTrees();

public:
    Database();
    ~Database();

    // Student Operations
    bool addStudent(const Student& s);
    Student* getStudent(const string& rollNo);
    // bool updateStudent(const Student& s);
    // bool deleteStudent(const string& rollNo);

    // Teacher Operations
    bool addTeacher(const Teacher& t);
    Teacher* getTeacher(const string& username);

    // Course Operations
    bool addCourse(const Course& c);
    Course* getCourse(const string& code);

    // Enrollment Operations
    bool enrollStudent(const string& rollNo, const string& courseCode, int semester);
    
    // Auth
    UserAuth* authenticate(const string& username, const string& password);
    
    // Persistence
    void save();
    void load();
    
    // Data Retrieval for Timetable
    vector<string> getAllCourseCodes();
    vector<Enrollment> getAllEnrollments();
    
    // Sample Data
    void generateSampleData();
};

#endif
